#
#  The OpenDiamond Platform for Interactive Search
#  Version 5
#
#  Copyright (c) 2011 Carnegie Mellon University
#  All rights reserved.
#
#  This software is distributed under the terms of the Eclipse Public
#  License, Version 1.0 which can be found in the file named LICENSE.
#  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
#  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

import base64

class Parameters(object):
    '''A list of formal parameters accepted by a Filter.'''
    def __init__(self, *params):
        self.params = params

    def __repr__(self):
        return '%s(%s)' % (self.__class__.__name__,
                        ', '.join(repr(p) for p in self.params))

    def describe(self):
        '''Return a dict describing the parameter list, suitable for
        opendiamond-manifest.txt.'''
        ret = {}
        for i in range(len(self.params)):
            info = self.params[i].describe()
            ret.update(('%s-%d' % (k, i), v) for k, v in info.iteritems())
        return ret

    def parse(self, args):
        '''Parse the specified argument list and return a list of parsed
        arguments.'''
        if len(self.params) != len(args):
            raise ValueError('Incorrect argument list length')
        return [self.params[i].parse(args[i]) for i in range(len(args))]


class BaseParameter(object):
    '''The base type for a formal parameter.'''
    type = 'unknown'

    def __init__(self, label, default=None):
        self.label = label
        self.default = default

    def __repr__(self):
        return '%s(%s, %s)' % (self.__class__.__name__, repr(self.label),
                            repr(self.default))

    def describe(self):
        ret = {
            'Label': self.label,
            'Type': self.type,
        }
        if self.default is not None:
            ret['Default'] = self.default
        return ret

    def parse(self, str):
        raise NotImplementedError()


class BooleanParameter(BaseParameter):
    '''A boolean formal parameter.'''
    type = 'boolean'

    def __init__(self, label, default=None):
        if default is not None:
            if default:
                default = 'true'
            else:
                default = 'false'
        BaseParameter.__init__(self, label, default)

    def parse(self, str):
        if str == 'true':
            return True
        elif str == 'false':
            return False
        else:
            raise ValueError('Argument must be true or false')


class StringParameter(BaseParameter):
    '''A string formal parameter.'''
    type = 'string'

    def parse(self, str):
        if str == '*':
            return ''
        else:
            return base64.b64decode(str)


class NumberParameter(BaseParameter):
    '''A number formal parameter.'''
    type = 'number'

    def __init__(self, label, default=None, min=None, max=None,
                        increment=None):
        BaseParameter.__init__(self, label, default)
        self.min = min
        self.max = max
        self.increment = increment

    def __repr__(self):
        return '%s(%s, %s, %s, %s, %s)' % (self.__class__.__name__,
                                repr(self.label), repr(self.default),
                                repr(self.min), repr(self.max),
                                repr(self.increment))

    def describe(self):
        ret = BaseParameter.describe(self)
        if self.min is not None:
            ret['Minimum'] = self.min
        if self.max is not None:
            ret['Maximum'] = self.max
        if self.increment is not None:
            ret['Increment'] = self.increment
        return ret

    def parse(self, str):
        val = float(str)
        if self.min is not None and val < self.min:
            raise ValueError('Argument too small')
        if self.max is not None and val > self.max:
            raise ValueError('Argument too large')
        return val


class ChoiceParameter(BaseParameter):
    '''A multiple-choice formal parameter.'''
    type = 'choice'

    def __init__(self, label, choices, default=None):
        '''choices is a tuple of (parsed-value, label) pairs'''
        if default is not None:
            for i, tag in enumerate(zip(*choices)[0]):
                if tag == default:
                    default = i
                    break
            else:
                raise ValueError('Default is not one of the choices')
        BaseParameter.__init__(self, label, default)
        self.choices = tuple(choices)

    def __repr__(self):
        return '%s(%s, %s, %s)' % (self.__class__.__name__,
                                repr(self.label), repr(self.choices),
                                repr(self.default))

    def describe(self):
        ret = BaseParameter.describe(self)
        for i in range(len(self.choices)):
            ret['Choice-%d' % i] = self.choices[i][1]
        return ret

    def parse(self, str):
        index = int(str)
        if index < 0 or index >= len(self.choices):
            raise ValueError('Selection out of range')
        return self.choices[index][0]