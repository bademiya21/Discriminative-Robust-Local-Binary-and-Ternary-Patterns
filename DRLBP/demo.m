% Demo script showing extraction of DRLBP feature for a 128 x 64 (h x w)
% pixel window

% Thanks to Piotr Dollar for his Matlab Toolbox(v3.01) which provided
% functions for gradient computation.

% Parameter initialization for DRLBP
cellpw = 16; % Block width
cellph = 16; % Block height
overlap = 0.5; % Block overlap
mapping = getmapping(8,'nrlbpu2'); % Mapping for robust lbp uniform codes
map(:,1) = mapping.table;
nbin = mapping.num; % RLBP bin number
clear mapping;
% calculate block slide step.
xbstride = cellpw*(1-overlap);
ybstride = cellph*(1-overlap);
% calculate the total blocks number in the window detected and pre-compute feature length.
feature_dim = (((128-cellph)/ybstride)+1)*(((64-cellpw)/xbstride)+1)*nbin*2;
radius = 1; % radius for LBP neighborhood

[gaussx, gaussy] = meshgrid(0:(cellpw-3), 0:(cellph-3)); % Weigh the gradients in the block using a gaussian spatial filter
gauss_weight = exp(-((gaussx-(cellpw-3)/2).*(gaussx-(cellpw-3)/2)+(gaussy-(cellph-3)/2).*(gaussy-(cellph-3)/2))/32);


% Image from INRIA Training (160 x 96 pixel in size)
img = imread('crop001158a.png');
[d1,d2,~] = size(img);

% Pre-processing
img1 = sqrt(double(img)/255);
[gradscal,~] = gradientMag(single(img1));
gradscal = double(gradscal);
clear img1;

%Extract valid detector window
gradscal = gradscal((d1-128)/2+1:end-(d1-128)/2,(d2-64)/2+1:end-(d2-64)/2);
[d1,d2,d3] = size(gradscal);
gradscal1 = permute(gradscal,[2 1 3]);
gradscal = reshape(gradscal1,d1*d2*d3,1);
clear gradscal1;

img = gamma_correct(img, 0.2);
    
%Extract valid detector window
[d1,d2,~] = size(img);
img = img((d1-128)*0.5+1:end-(d1-128)*0.5,(d2-64)*0.5+1:end-(d2-64)*0.5,:);

[d1,d2,d3] = size(img);
img1 = permute(double(img),[2 1 3]);
img = reshape(img1,d1*d2*d3,1);
clear img1;

% Call LBP_Opt to generate the feature
% USAGE
% feature = LBP_Opt(img,map,gradscal,gauss_weight,d1,d2,3,feature_dim,nbin,cellpw,cellph,xbstride,ybstride,radius)
%
% INPUTS
%  img              - [hxwx3] input 3 channel single image
%  map              - mapping table obtained from getmapping
%  gradscal         - gradient magnitude of window in raster-scan order in 1-D
%                     matrix
%  gauss_weight     - gaussian spatial filter (optional - use all 1s if not
%                     required)
%  d1,d2,3          - dimension of image window (3rd dimension is always 3)
%  feature_dim      - feature dimension size (look above how to
%                     pre-compute)
%  nbin             - RLBP bin number (can be obtained from mapping)
%  cellpw           - Block width
%  cellph           - Block height
%  xbstride,ybstride- Block slide steps in x and y directions
%  radius           - radius for neighborhood

feature = LBP_Opt(img,map,gradscal,gauss_weight,d1,d2,3,feature_dim,nbin,cellpw,cellph,xbstride,ybstride,radius); % Call mex function to generate feature