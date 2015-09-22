% generate fft data for autotests

function gen_fft_data(n);
n = round(n);

x = randn(1,n) + j*randn(1,n);
y = fft(x);

% print results

filename = ['fft_data_' num2str(n) '.c'];
fid = fopen(filename,'w');

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
LOGI('// autotest fft data for %u-point transform\n', n);
LOGI('//\n');
LOGI('\n');
LOGI('#include <complex.h>\n');
LOGI('\n');

LOGI('float complex fft_test_x%d[] = {\n',n);
for i=1:n,
    LOGI('  %16.12f + %16.12f*_Complex_I', real(x(i)), imag(x(i)));
    if i==n,
        LOGI('};\n');
    else,
        LOGI(',\n');
    end;
end;

LOGI('\n');

LOGI('float complex fft_test_y%d[] = {\n',n);
for i=1:n,
    LOGI('  %16.12f + %16.12f*_Complex_I', real(y(i)), imag(y(i)));
    if i==n,
        LOGI('};\n');
    else,
        LOGI(',\n');
    end;
end;

LOGI('\n');

fclose(fid);
LOGI('results written to %s\n', filename);
