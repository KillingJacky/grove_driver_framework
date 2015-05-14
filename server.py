#!/usr/bin/python

#   Copyright (C) 2015 by seeedstudio
#   Author: Jack Shao (jacky.shaoxg@gmail.com)
#
#   The MIT License (MIT)
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to deal
#   in the Software without restriction, including without limitation the rights
#   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#   copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#   THE SOFTWARE.

#   Dependences: pip install tornado

from datetime import timedelta
import socket
import json

from tornado.web import RequestHandler, Application, url
from tornado.httpserver import HTTPServer
from tornado.tcpserver import TCPServer
from tornado import ioloop
from tornado import gen
from tornado import iostream


class DeviceConnection(object):

    recv_msg_queue = []
    send_msg_queue = []

    def __init__ (self, device_server, stream, address):
        self.device_server = device_server
        self.stream = stream
        self.address = address
        self.stream.set_nodelay(True)
        self.node_id = ""

    @gen.coroutine
    def wait_hello (self):
        try:
            self._wait_hello_future = self.stream.read_until("hello\r\n")
            str = yield gen.with_timeout(timedelta(seconds=10), self._wait_hello_future,
                                         io_loop=self.stream.io_loop)
            self.stream.write("hello\r\n")
            raise gen.Return(0)
        except gen.TimeoutError:
            self.kill_myself()
            raise gen.Return(1)
        except iostream.StreamClosedError:
            self.kill_myself()
            raise gen.Return(2)

        #self.stream.io_loop.add_future(self._serving_future, lambda future: future.result())
    @gen.coroutine
    def wait_node_id (self):
        try:
            msg = yield self.stream.read_until("\r\n")
            msg = msg.strip("\r").strip("\n").strip("\r\n")
            self.node_id = msg
            print "node id:", msg
            raise gen.Return(0)
        except iostream.StreamClosedError:
            self.kill_myself()
            raise gen.Return(1)

    @gen.coroutine
    def _loop_reading_input (self):
        while True:
            msg = ""
            try:
                msg = yield self.stream.read_until("\r\n")
                msg = msg.strip("\r\n")
                json_obj = json.loads(msg)
                print json_obj
                self.recv_msg_queue.append(json_obj)
                print self.recv_msg_queue
            except iostream.StreamClosedError:
                self.kill_myself()
                return
            except ValueError:
                print msg, " can not be decoded into json"
                pass

            yield gen.moment

    @gen.coroutine
    def _loop_sending_cmd (self):
        while True:
            if len(self.send_msg_queue) > 0:
                try:
                    cmd = self.send_msg_queue.pop()
                    yield self.stream.write(cmd)
                except Exception, e:
                    yield gen.moment
            else:
                yield gen.sleep(0.1)

    @gen.coroutine
    def start_serving (self):
        ret = yield self.wait_hello()
        if ret == 0:
            print "waited hello"
        elif ret == 1:
            print "timeout waiting hello, kill this connection"
            return
        elif ret == 2:
            print "connection is closed by client"
            return

        ret = yield self.wait_node_id()
        if ret == 1:
            print "connection is closed by client"
            return

        ## check node_id if exists in the database
        ## assume true

        ## loop reading the stream input
        self._loop_reading_input()
        self._loop_sending_cmd()

    def kill_myself (self):
        self.stream.io_loop.add_callback(self.device_server.remove_connection, self)
        self.stream.close()

    def submit_cmd (self, cmd):
        self.send_msg_queue.append(cmd)

    @gen.coroutine
    def submit_and_wait_resp (self, cmd, target_resp, timeout_sec=10):
        self.submit_cmd(cmd)
        timeout = 0
        while True:
            try:
                for msg in self.recv_msg_queue:
                    if msg['msg_type'] == target_resp:
                        tmp = msg['msg']
                        self.recv_msg_queue.remove(msg)
                        raise gen.Return((True, tmp))
                yield gen.sleep(0.1)
                timeout += 0.1
                if timeout > timeout_sec:
                    raise gen.Return((False, "timeout when waiting response from node"))
            except gen.Return:
                raise
            except Exception,e:
                print e



class DeviceServer(TCPServer):

    accepted_conns = []

    def handle_stream(self, stream, address):
        conn = DeviceConnection(self, stream,address)
        self.accepted_conns.append(conn)
        conn.start_serving()
        print "accepted conns: ", len(self.accepted_conns)

    def remove_connection (self, conn):
        print "will remove connection: ", conn
        self.accepted_conns.remove(conn)


class IndexHandler(RequestHandler):
    def get(self):
        DeviceServer.accepted_conns[0].submit_cmd("OTA\r\n")
        self.write("Please specify the url as this format: /node_id/grove_name/property")

class NodeReadWriteHandler(RequestHandler):

    @gen.coroutine
    def get(self, node_id, grove_name, method):
        print "get",node_id, grove_name,method

        for conn in DeviceServer.accepted_conns:
            if conn.node_id == node_id:
                try:
                    cmd = "GET /%s/%s\r\n"%(grove_name, method)
                    cmd = cmd.encode("ascii")
                    ok, resp = yield conn.submit_and_wait_resp (cmd, "resp_get")
                    self.write(resp)
                except Exception,e:
                    print e
                return

        self.write("\r\n")
        self.write("Node is offline\r\n")

    @gen.coroutine
    def post (self, node_id, grove_name, method):
        print "post", node_id, grove_name, method, self.request.body

        if self.request.body.find("&") > 0:
            arg_list = self.request.body.split('&')
        else:
            arg_list = self.request.body.split(',')

        if arg_list[0] == "":
            self.write("Bad format of the post body\r\nAllowed format:\r\n")
            self.write("1. arg1=value1&arg2=value2 in order\r\n")
            self.write("2. value1,value2 in order\r\n")
            return

        cmd_args = ""
        for arg in arg_list:
            if arg.find('=') > -1:
                value = arg.split('=')[1]
                cmd_args += value
                cmd_args += "/"
            else:
                cmd_args += arg
                cmd_args += "/"


        for conn in DeviceServer.accepted_conns:
            if conn.node_id == node_id:
                try:
                    cmd = "POST /%s/%s/%s\r\n"%(grove_name, method, cmd_args)
                    cmd = cmd.encode("ascii")
                    ok, resp = yield conn.submit_and_wait_resp (cmd, "resp_post")
                    self.write(resp)
                except Exception,e:
                    print e
                return

        self.write("\r\n")
        self.write("Node is offline\r\n")


def main():

    app = Application([
        url(r"/", IndexHandler),
        url(r"/([a-zA-Z0-9]+)/(.+)/(.+)[/]?", NodeReadWriteHandler),
        ])
    http_server = HTTPServer(app)
    http_server.listen(8080)

    tcp_server = DeviceServer()
    tcp_server.listen(8081)

    ioloop.IOLoop.current().start()

if __name__ == '__main__':
    main()

