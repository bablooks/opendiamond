
/*
 *      OpenDiamond 2.0
 *      A system for interactive brute-force search
 *
 *      Copyright (c) 2002-2005, Intel Corporation
 *      All Rights Reserved
 *
 *  This software is distributed under the terms of the Eclipse Public
 *  License, Version 1.0 which can be found in the file named LICENSE.
 *  ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 *  RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */



static char const cvsid[] =
    "$Header$";

extern int      read_gid_map(char *file);

int
main(int argc, char **argv)
{

	read_gid_map("gid_map");

}
