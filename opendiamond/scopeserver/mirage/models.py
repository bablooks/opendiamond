#
#  The OpenDiamond Platform for Interactive Search
#
#  Copyright (c) 2009 Carnegie Mellon University
#  All rights reserved.
#
#  This software is distributed under the terms of the Eclipse Public
#  License, Version 1.0 which can be found in the file named LICENSE.
#  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
#  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

from django.db import models


class Access(models.Model):
    # pylint: disable=model-missing-unicode
    class Meta:
        permissions = (("search", "Can search VM images"),)
