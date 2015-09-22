%
% write_header(fid,comment)
%
% write copyright and header to file descriptor 'fid' with optional comment
%
function gendata_write_header(fid,comment)

if nargin < 2,
    comment = 'auto-generated data file';
end;

LOGI('/*\n');
LOGI(' * Copyright (c) 2007 - 2014 Joseph Gaeddert\n');
LOGI(' *\n');
LOGI(' * This file is part of liquid.\n');
LOGI(' *\n');
LOGI(' * liquid is free software: you can redistribute it and/or modify\n');
LOGI(' * it under the terms of the GNU General Public License as published by\n');
LOGI(' * the Free Software Foundation, either version 3 of the License, or\n');
LOGI(' * (at your option) any later version.\n');
LOGI(' *\n');
LOGI(' * liquid is distributed in the hope that it will be useful,\n');
LOGI(' * but WITHOUT ANY WARRANTY; without even the implied warranty of\n');
LOGI(' * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n');
LOGI(' * GNU General Public License for more details.\n');
LOGI(' *\n');
LOGI(' * You should have received a copy of the GNU General Public License\n');
LOGI(' * along with liquid.  If not, see <http://www.gnu.org/licenses/>.\n');
LOGI(' */\n');
LOGI('\n');
LOGI('//\n');
LOGI('// %s\n', comment);
LOGI('//\n');
LOGI('\n');
LOGI('#include <complex.h>\n');
LOGI('\n');

