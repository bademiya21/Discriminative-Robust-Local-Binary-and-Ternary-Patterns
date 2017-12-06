% Demo script showing extraction of DRLTP feature for a 128 x 64 (h x w)
% pixel window

% Thanks to Piotr Dollar for his Matlab Toolbox(v3.01) which provided
% functions for gradient computation.

% Parameter initialization for DRLTP
cellpw = 16;
cellph = 16;
overlap = 0.5;
mapping = getmapping_ltp(8,'nrltp');
map_upper(:,1) = mapping.table_upper;
map_lower(:,1) = mapping.table_lower;
nbin1 = mapping.numupper;
nbin2 = mapping.num1ower;
clear mapping;
% calculate block slide step.
xbstride = cellpw*(1-overlap);
ybstride = cellph*(1-overlap);
% calculate the total blocks number in the window detected and pre-compute feature length.
feature_dim = (((128-cellph)/ybstride)+1)*(((64-cellpw)/xbstride)+1)*(nbin1+nbin2)*2;
radius = 1; % radius for LTP neighborhood
LTP_threshold = 1;

[gaussx, gaussy] = meshgrid(0:(cellpw-3), 0:(cellph-3));
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

img = gamma_correct(img, 0.4);
    
%Extract valid detector window
[d1,d2,~] = size(img);
img = img((d1-128)*0.5+1:end-(d1-128)*0.5,(d2-64)*0.5+1:end-(d2-64)*0.5,:);

[d1,d2,d3] = size(img);
img1 = permute(double(img),[2 1 3]);
img = reshape(img1,d1*d2*d3,1);
clear img1;

% Call LTP_Opt to generate the feature
% USAGE
% feature = LTP_Opt(img,map_upper,map_lower,gradscal,gauss_weight,d1,d2,3,feature_dim,nbin1,nbin2,cellpw,cellph,xbstride,ybstride,radius,LTP_threshold)
%
% INPUTS
%  img              - [hxwx3] input 3 channel single image
%  map_upper        - mapping table for split upper LBP histogram obtained from getmapping
%  map_lower        - mapping table for split lower LBP histogram obtained from getmapping
%  gradscal         - gradient magnitude of window in raster-scan order in 1-D
%                     matrix
%  gauss_weight     - gaussian spatial filter (optional - use all 1s if not
%                     required)
%  d1,d2,3          - dimension of image window (3rd dimension is always 3)
%  feature_dim      - feature dimension size (look above how to
%                     pre-compute)
%  nbin1,nbin2      - RLTP bin number (can be obtained from mapping)
%  cellpw           - Block width
%  cellph           - Block height
%  xbstride,ybstride- Block slide steps in x and y directions
%  radius           - radius for neighborhood
%  LTP_threshold    - Positive threshold for LTP

feature = LTP_Opt(img,map_upper,map_lower,gradscal,gauss_weight,d1,d2,3,feature_dim,nbin1,nbin2,cellpw,cellph,xbstride,ybstride,radius,LTP_threshold); % Call mex function to generate feature