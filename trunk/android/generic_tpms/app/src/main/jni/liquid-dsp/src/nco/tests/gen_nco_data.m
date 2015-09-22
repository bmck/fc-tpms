% generate nco data for autotests

% options
p = 7;          % base frequency integer
f = 1/sqrt(p);  % frequency
n = 256;        % number of points

% compute sincos data
v = exp(j*[0:(n-1)]*f);

% print results

basename = ['nco_sincos_fsqrt1_' num2str(p)];
filename = ['data/' basename '.c'];
fid      = fopen(filename,'w');

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
LOGI('// autotest nco sincos data for f=%.12f\n', f);
LOGI('//\n');

LOGI('\n');
LOGI('#include <complex.h>\n');
LOGI('\n');

LOGI('float complex %s[%u] = {\n', basename, n);
for i=1:n,
    %LOGI('  {%16.12f, %16.12f}', real(v(i)), imag(v(i)));
    LOGI('  %16.12f + _Complex_I*%16.12f', real(v(i)), imag(v(i)));
    if i==n,
        LOGI('};\n');
    else,
        LOGI(',\n');
    end;
end;

LOGI('\n');

fclose(fid);
LOGI('results written to %s\n', filename);
