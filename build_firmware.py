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

#dependence: PyYAML (pip install PyYaml)


import os
import sys
import re
import json

import yaml

GEN_DIR = '.'

TYPE_MAP = {
    'int':'TYPE_INT',
    'float':'TYPE_FLOAT',
    'bool':'TYPE_BOOL',
    'uint8_t':'TYPE_UINT8',
    'int8_t':'TYPE_INT8',
    'uint16_t':'TYPE_UINT16',
    'int16_t':'TYPE_INT16',
    'uint32_t':'TYPE_UINT32',
    'int32_t':'TYPE_INT32',
    'const char *':'TYPE_STRING',
    'char *':'TYPE_STRING'
    }


def find_grove_in_database (grove_name, json_obj):
    for grove in json_obj:
        if grove['GroveName'] == grove_name:
            return grove
    return {}

def declare_vars (arg_list):
    result = ""
    for arg in arg_list:
        result += arg.replace('*','')
        result += ';\r\n    '
    return result

def build_read_call_args (arg_list):
    result = ""
    for arg in arg_list:
        result += arg.strip().split(' ')[1].replace('*', '&')
        result += ','
    return result.rstrip(',')

def build_read_print (arg_list):
    result = ""
    cnt = len(arg_list)
    for i in xrange(cnt):
        t = arg_list[i].strip().split(' ')[0]
        if t in TYPE_MAP.keys():
            result += "        writer_print(%s, %s%s);\r\n" %(TYPE_MAP[t], arg_list[i].strip().split(' ')[1].replace('*', '&'), \
                                                              ', true' if i < (cnt-1) else '')
        else:
            print 'arg type %s not supported' % t
            sys.exit()
    return result

def build_unpack_vars (arg_list):
    result = ""
    for arg in arg_list:
        t = arg.strip().split(' ')[0]
        name = arg.strip().split(' ')[1]
        result += '    %s = *((%s *)arg_ptr); arg_ptr += sizeof(%s);\r\n' % (name, t, t)
    return result;

def build_reg_write_arg_type (arg_list):
    result = ""
    for i in xrange(len(arg_list)):
        t = arg_list[i].strip().split(' ')[0]
        if t in TYPE_MAP.keys():
            result += "    arg_types[%d] = %s;\r\n" %(i, TYPE_MAP[t])
        else:
            print 'arg type %s not supported' % t
            sys.exit()
    return result


def gen_wrapper_registration (instance_name, info, arg_list):
    grove_name = info['GroveName'].lower()
    gen_header_file_name = grove_name+"_gen.h"
    gen_cpp_file_name = grove_name+"_gen.cpp"
    fp_wrapper_h = open(os.path.join(GEN_DIR, gen_header_file_name),'w')
    fp_wrapper_cpp = open(os.path.join(GEN_DIR, gen_cpp_file_name), 'w')
    str_reg_include = ""
    str_reg_method = ""

    #leading part
    fp_wrapper_h.write('#include "%s"\r\n\r\n' % info['ClassFile'])
    fp_wrapper_cpp.write('#include "%s"\r\n#include "%s"\r\n#include "%s"\r\n\r\n' % (gen_header_file_name, "rpc_server.h", "rpc_stream.h"))
    str_reg_include += '#include "%s"\r\n\r\n' % gen_header_file_name
    args_in_string = ""
    for arg in info['ConstructArgList']:
        arg_name = arg.strip().split(' ')[1]
        if arg_name in arg_list.keys():
            args_in_string += ","
            args_in_string += arg_list[arg_name]
        else:
            print "ERR: no construct arg name in config file matchs %s" % arg_name
            sys.exit()
    str_reg_method += '    %s *%s = new %s(%s);\r\n' % (info['ClassName'], instance_name, info['ClassName'], args_in_string.lstrip(","))


    #loop part
    #read functions
    str_reg_method += '    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);\r\n'

    for fun in info['Outputs'].items():
        fp_wrapper_h.write('void __%s_%s(void *class_ptr, void *input);\r\n' % (grove_name, fun[0]))

        fp_wrapper_cpp.write('void __%s_%s(void *class_ptr, void *input)\r\n' % (grove_name, fun[0]))
        fp_wrapper_cpp.write('{\r\n')
        fp_wrapper_cpp.write('    %s *grove = (%s *)class_ptr;\r\n' % (info['ClassName'], info['ClassName']))
        fp_wrapper_cpp.write('    %s\r\n'%declare_vars(fun[1]))
        fp_wrapper_cpp.write('    if(grove->%s(%s))\r\n'%(fun[0],build_read_call_args(fun[1])))
        fp_wrapper_cpp.write('    {\r\n')
        fp_wrapper_cpp.write(build_read_print(fun[1]))
        fp_wrapper_cpp.write('    }else\r\n')
        fp_wrapper_cpp.write('    {\r\n')
        fp_wrapper_cpp.write('        writer_print(TYPE_STRING, "Failed");\r\n')
        fp_wrapper_cpp.write('    }\r\n')
        fp_wrapper_cpp.write('}\r\n\r\n')

        str_reg_method += '    rpc_server_register_method("%s", "%s", METHOD_READ, %s, %s, arg_types);\r\n' % \
                          (instance_name, fun[0].replace('read_',''), '__'+grove_name+'_'+fun[0], instance_name)

    #write functions
    for fun in info['Inputs'].items():
        fp_wrapper_h.write('void __%s_%s(void *class_ptr, void *input);\r\n' % (grove_name, fun[0]))

        fp_wrapper_cpp.write('void __%s_%s(void *class_ptr, void *input)\r\n' % (grove_name, fun[0]))
        fp_wrapper_cpp.write('{\r\n')
        fp_wrapper_cpp.write('    %s *grove = (%s *)class_ptr;\r\n' % (info['ClassName'], info['ClassName']))
        fp_wrapper_cpp.write('    uint8_t *arg_ptr = (uint8_t *)input;\r\n')
        fp_wrapper_cpp.write('    %s\r\n' % declare_vars(fun[1]))
        fp_wrapper_cpp.write(build_unpack_vars(fun[1]))
        fp_wrapper_cpp.write('\r\n')
        fp_wrapper_cpp.write('    if(grove->%s(%s))\r\n'%(fun[0],build_read_call_args(fun[1])))
        fp_wrapper_cpp.write('        writer_print(TYPE_STRING, "OK");\r\n')
        fp_wrapper_cpp.write('    else\r\n')
        fp_wrapper_cpp.write('        writer_print(TYPE_STRING, "Failed");\r\n')
        fp_wrapper_cpp.write('}\r\n\r\n')

        str_reg_method += '    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);\r\n'
        str_reg_method += build_reg_write_arg_type(fun[1])
        str_reg_method += '    rpc_server_register_method("%s", "%s", METHOD_WRITE, %s, %s, arg_types);\r\n' % \
                          (instance_name, fun[0].replace('write_',''), '__'+grove_name+'_'+fun[0], instance_name)

    # event attachment
    if info['HasEvent']:
        str_reg_method += '\r\n    %s->attach_event_handler(rpc_server_event_report);\r\n' % instance_name

    fp_wrapper_h.close()
    fp_wrapper_cpp.close()
    return (str_reg_include, str_reg_method)



if __name__ == '__main__':

    ###generate rpc wrapper and registration files

    cur_dir = os.path.split(os.path.realpath(__file__))[0]
    f_database = open('%s/database.json' % cur_dir,'r')
    js = json.load(f_database)

    f_config = open('connection_config.yaml','r')
    config = yaml.load(f_config)

    if not os.path.exists(GEN_DIR):
        os.mkdir(GEN_DIR)

    fp_reg_cpp = open(os.path.join(GEN_DIR, "rpc_server_registration.cpp"),'w')

    for grove_instance_name in config.keys():
        grove = find_grove_in_database(config[grove_instance_name]['name'], js)
        if grove:
            str_reg_include, str_reg_method = \
            gen_wrapper_registration(grove_instance_name, grove, config[grove_instance_name]['construct_arg_list'])
            #print str_reg_include
            #print str_reg_method
            fp_reg_cpp.write('#include "suli2.h"\r\n')
            fp_reg_cpp.write('#include "rpc_server.h"\r\n\r\n')
            fp_reg_cpp.write(str_reg_include)
            fp_reg_cpp.write('\r\n')
            fp_reg_cpp.write('void rpc_server_register_resources()\r\n')
            fp_reg_cpp.write('{\r\n')
            fp_reg_cpp.write('    uint8_t arg_types[MAX_INPUT_ARG_LEN];\r\n')
            fp_reg_cpp.write('    \r\n')
            fp_reg_cpp.write('    //%s\r\n'%grove_instance_name)
            fp_reg_cpp.write(str_reg_method)
            fp_reg_cpp.write('}\r\n')

        else:
            print "can not find %s in database"%grove_instance_name
            sys.exit()

    fp_reg_cpp.close()

    ### make



