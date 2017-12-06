%GETMAPPING returns a structure containing a mapping table for LBP codes.
%  MAPPING = GETMAPPING(SAMPLES,MAPPINGTYPE) returns a 
%  structure containing a mapping table for
%  LBP codes in a neighbourhood of SAMPLES sampling
%  points. Possible values for MAPPINGTYPE are
%       'u2'   for uniform LBP
%       'ri'   for rotation-invariant LBP
%       'riu2' for uniform rotation-invariant LBP.
%
%  Example:
%       I=imread('rice.tif');
%       MAPPING=getmapping(16,'riu2');
%       LBPHIST=lbp(I,2,16,MAPPING,'hist');
%  Now LBPHIST contains a rotation-invariant uniform LBP
%  histogram in a (16,2) neighbourhood.
%

function mapping = getmapping_ltp(samples,mappingtype)
% Version 0.1.1
% Authors: Marko Heikkilä and Timo Ahonen

% Changelog
% 0.1.1 Changed output to be a structure
% Fixed a bug causing out of memory errors when generating rotation 
% invariant mappings with high number of sampling points.
% Lauge Sorensen is acknowledged for spotting this problem.

if strcmp(mappingtype,'nrltp')
    ternary_code = 0:3^samples-1;
    ternary_code = dec2base(ternary_code,3,samples);

    for i=1:3^samples
        for j=1:samples
            if (ternary_code(i,j) == '0')
                ternary_code_nr(i,j) = '2';
            end
            if (ternary_code(i,j) == '1')
                ternary_code_nr(i,j) = '1';
            end
            if (ternary_code(i,j) == '2')
                ternary_code_nr(i,j) = '0';
            end
        end
        ternary_code_nr(i,:) = dec2base(min(base2dec(ternary_code(i,:),3),base2dec(ternary_code_nr(i,:),3)),3,samples);
    end
    clear ternary_code;
    
    map_upper = zeros(1,3^samples);
    map_lower = zeros(1,3^samples);
    
    for i=1:3^samples
        for j=1:samples
            if (ternary_code_nr(i,j) == '0')
                map_lower(i) = map_lower(i) + 2^(samples-j);
            end
            if (ternary_code_nr(i,j) == '2')
                map_upper(i) = map_upper(i) + 2^(samples-j);
            end
        end
    end
    uniform_code = getmapping(samples,'u2');
    nrlbp_code = getmapping(samples,'nrlbpu2');
    for i=1:3^samples
        map_lower(i) = uniform_code.table(map_lower(i)+1);
        map_upper(i) = nrlbp_code.table(map_upper(i)+1);
    end
    clear ternary_code_nr;
    mapping.table_upper=map_upper;
    mapping.table_lower=map_lower;
    mapping.samples=samples;
    mapping.num1ower=uniform_code.num;
    mapping.numupper=nrlbp_code.num;
end

if strcmp(mappingtype,'ltp')
    ternary_code = 0:3^samples-1;
    ternary_code = dec2base(ternary_code,3,samples);
    
    map_upper = zeros(1,3^samples);
    map_lower = zeros(1,3^samples);
    
    for i=1:3^samples
        for j=1:samples
            if (ternary_code(i,j) == '0')
                map_lower(i) = map_lower(i) + 2^(samples-j);
            end
            if (ternary_code(i,j) == '2')
                map_upper(i) = map_upper(i) + 2^(samples-j);
            end
        end
    end
    uniform_code = getmapping(samples,'u2');
    for i=1:3^samples
        map_lower(i) = uniform_code.table(map_lower(i)+1);
        map_upper(i) = uniform_code.table(map_upper(i)+1);
    end
    clear ternary_code;
    mapping.table_upper=map_upper;
    mapping.table_lower=map_lower;
    mapping.samples=samples;
    mapping.num1ower=uniform_code.num;
    mapping.numupper=uniform_code.num;
end
