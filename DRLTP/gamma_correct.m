function [J, varargout] = gamma_correct(I, gamma)
%
% GAMMA_CORRECT   applies gamma LUT to input image
%    J = GAMMA_CORRECT(I, gamma) or [J, LUT] = GAMMA_CORRECT(I, gamma)
%
%    J: output image, of the same type as I
%    I: input image
%    LUT: gamma correction lookup-table (transform function)
%
 
max_intensity = max_pix_val(I);
 
% create lookup table based on gamma (must take care to scale
% table elements correctly.
LUT = max_intensity .* ( ((0:max_intensity)./max_intensity).^gamma );
% LUT = floor(LUT); % floor instead of round to simulate C code
LUT = round(LUT);
 
% apply LUT to all image planes - here I utilize a MATLAB 'trick'
% to elegantly apply the indexing operation (the +1 is because
% MATLAB is ones-based!)
J = LUT(double(I)+1); % convert I to double b/c MATLAB only supports
                      % vectorized indexing on double types

J = cast(J, class(I));
 
% return the transform function if the user asked for it.
if 2 == nargout
    varargout{1} = LUT;
end
 
function v = max_pix_val(I)
%
% A little helper sub-function that given an image 
% matrix I, returns the maximum pixel value based on
% I's class.
%
 
if isa(I, 'double') || isa(I, 'single')
    v = 1.0;
elseif isa(I, 'uint8')
    v = 255;
elseif isa(I, 'uint16')
    v = 65535;
else
    error('unsupported class type %s', class(I));
end