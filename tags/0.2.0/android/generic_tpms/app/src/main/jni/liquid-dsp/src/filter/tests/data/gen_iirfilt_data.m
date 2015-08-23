%
% generate iirfilt data for autotests
%

clear all;
close all;

%
% function to generate data
%
function gen_iirfilt_datafile(type,h_len,x_len);

    % determine type
    x_complex = 0;
    h_complex = 0;
    y_complex = 0;
    if strcmp(type,'rrr'),
        x_complex = 0;
        h_complex = 0;
        y_complex = 0;
    elseif strcmp(type,'crc'),
        x_complex = 1;
        h_complex = 0;
        y_complex = 1;
    elseif strcmp(type,'ccc'),
        x_complex = 1;
        h_complex = 1;
        y_complex = 1;
    else,
        error(['invalid/unsupported type (' type ')']);
    end;

    %
    % generate low-pass butterworth filter coefficients
    %

    % generate complex roots
    n = h_len - 1;      % filter order
    r = mod(n, 2);      % odd/even order
    L = (n-r)/2;        % filter semi-length
    pa = zeros(1,n);    % analog poles (roots of a)
    for i=0:(L-1),
        theta = (2*(i+1) + n - 1)*pi / (2*n);
        pa(2*i+1) = exp( j*theta );
        pa(2*i+2) = exp(-j*theta );
    end;
    if r,
        pa(end) = -1;
    end;

    % convert analog poles/zeros/gain to digital
    fc = 0.1;
    m  = abs(tan(pi*fc));
    p = zeros(1,n);
    z = zeros(1,n);
    k = 1;
    for i=1:n,
        % compute digital zeros...
        z(i) = -1;

        % compute digital pols
        pm = pa(i)*m;
        p(i) = (1 + pm)/(1 - pm);

        % compute digital gain
        k = k*(1 - p(i)) / (1 - z(i));
    end;

    % expand roots
    b = [1];
    a = [1];
    k = 1;  % gain
    for i=1:n,
        b = conv(b, [1, -z(i)]);
        a = conv(a, [1, -p(i)]);
        k = k*(1 - p(i))/(1 - z(i));
    end;
    k = real(k);
    a = real(a);
    b = real(b) * k;

    % modulate coefficients for complex type
    if h_complex,
        for i=1:h_len,
            a(i) = a(i) * exp(j*2*pi*0.1*i);
            b(i) = b(i) * exp(j*2*pi*0.1*i);
        end;
    end;

    % generate input data
    if x_complex,
        x = 0.1*[randn(1,x_len) + 1i*randn(1,x_len)];
    else,
        x = 0.1*[randn(1,x_len)];
    end;

    % filter input
    y = filter(b,a,x);
    y_len = length(y);

    % print results
    % filename example: iirfilt_crcf_data_h12x44.c
    filename = ['iirfilt_' type 'f_data_h' num2str(h_len) 'x' num2str(x_len) '.c'];
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
    LOGI('// %s: autotest iirfilt data\n', filename);
    LOGI('//\n');
    LOGI('\n');
    if h_complex || x_complex || y_complex,
        LOGI('#include <complex.h>\n\n');
    end;

    % construct base name, e.g. 'iirfilt_crcf_test_h12x44'
    basename = ['iirfilt_' type 'f_data_h' num2str(h_len) 'x' num2str(x_len)];

    % save coefficients arrays
    if h_complex, LOGI('float complex ');
    else,         LOGI('float ');
    end;
    LOGI('%s_b[] = {\n', basename);
    for i=1:h_len,
        if h_complex, LOGI('  %16.12f + %16.12f*_Complex_I', real(b(i)), imag(b(i)));
        else,         LOGI('  %16.12f', b(i));
        end;

        if i==h_len,  LOGI('};\n\n');
        else,         LOGI(',\n');
        end;
    end;

    % save coefficients arrays
    if h_complex, LOGI('float complex ');
    else,         LOGI('float ');
    end;
    LOGI('%s_a[] = {\n', basename);
    for i=1:h_len,
        if h_complex, LOGI('  %16.12f + %16.12f*_Complex_I', real(a(i)), imag(a(i)));
        else,         LOGI('  %16.12f', a(i));
        end;

        if i==h_len,  LOGI('};\n\n');
        else,         LOGI(',\n');
        end;
    end;

    % save input array
    if x_complex, LOGI('float complex ');
    else,         LOGI('float ');
    end;
    LOGI('%s_x[] = {\n', basename);
    for i=1:x_len,
        if x_complex, LOGI('  %16.12f + %16.12f*_Complex_I', real(x(i)), imag(x(i)));
        else,         LOGI('  %16.12f', x(i));
        end;

        if i==x_len,  LOGI('};\n\n');
        else,         LOGI(',\n');
        end;
    end;

    % save output array
    if y_complex, LOGI('float complex ');
    else,         LOGI('float ');
    end;
    LOGI('%s_y[] = {\n', basename);
    for i=1:y_len,
        if y_complex, LOGI('  %16.12f + %16.12f*_Complex_I', real(y(i)), imag(y(i)));
        else,         LOGI('  %16.12f', y(i));
        end;

        if i==y_len,  LOGI('};\n\n');
        else,         LOGI(',\n');
        end;
    end;

    fclose(fid);
    LOGI('results written to %s\n', filename);

endfunction;

%
% generate test data
%

% rrrf data
gen_iirfilt_datafile('rrr', 3, 64);
gen_iirfilt_datafile('rrr', 5, 64);
gen_iirfilt_datafile('rrr', 7, 64);

% crcf data
gen_iirfilt_datafile('crc', 3, 64);
gen_iirfilt_datafile('crc', 5, 64);
gen_iirfilt_datafile('crc', 7, 64);

% cccf data
gen_iirfilt_datafile('ccc', 3, 64);
gen_iirfilt_datafile('ccc', 5, 64);
gen_iirfilt_datafile('ccc', 7, 64);


