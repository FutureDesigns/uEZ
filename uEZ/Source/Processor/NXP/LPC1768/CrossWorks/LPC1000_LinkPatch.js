/******************************************************************************
  Copyright (c) 2009 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function patch(filename)
{
  backupFilename = filename + ".prepatch";
  CWSys.removeFile(backupFilename);
  CWSys.renameFile(filename, backupFilename);
  ElfFile.load(backupFilename);
  crc = 0;
  for (i = 0x00; i < 0x1C; i += 4)
    crc += ElfFile.peekUint32(i);
  ElfFile.pokeUint32(0x1C, ~crc + 1);
  ElfFile.save(filename);
}

