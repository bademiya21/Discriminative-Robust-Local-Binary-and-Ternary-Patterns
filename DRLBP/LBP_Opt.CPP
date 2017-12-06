#include "mex.h"
#include "LBP.h"
#include <iostream>
#include <string.h>

using namespace std;

integerpoint points[bits];
doublepoint offsets[bits];
const double M_PI = acos((double) - 1);

/*
 * Get a bilinearly interpolated value for a pixel.
 */
inline double interpolate_at_ptr(double* upperLeft, int i, int columns)
{
    double dx = 1-offsets[i].x;
    double dy = 1-offsets[i].y;
    return
            *upperLeft*dx*dy +
            *(upperLeft+1)*offsets[i].x*dy +
            *(upperLeft+columns)*dx*offsets[i].y +
            *(upperLeft+columns+1)*offsets[i].x*offsets[i].y;
}

// /*
//  * Calculate the point coordinates for circular sampling of the neighborhood.
//  */
void calculate_points(void)
{
    double step = 2 * M_PI / bits, tmpX, tmpY;
    int i;
    for (i=0;i<bits;i++)
    {
        tmpX = predicate * cos(i * step);
        tmpY = predicate * sin(i * step);
        points[i].x = (int)tmpX;
        points[i].y = (int)tmpY;
        offsets[i].x = tmpX - points[i].x;
        offsets[i].y = tmpY - points[i].y;
        if (offsets[i].x < 1.0e-10 && offsets[i].x > -1.0e-10) /* rounding error */
            offsets[i].x = 0;
        if (offsets[i].y < 1.0e-10 && offsets[i].y > -1.0e-10) /* rounding error */
            offsets[i].y = 0;
        
        if (tmpX < 0 && offsets[i].x != 0)
        {
            points[i].x -= 1;
            offsets[i].x += 1;
        }
        if (tmpY < 0 && offsets[i].y != 0)
        {
            points[i].y -= 1;
            offsets[i].y += 1;
        }
    }
}

void lbp_histogram_opt(double* img, double* feature, double* map, int rows, int columns, int channel, int nbin, int xsize, int ysize, int xbstride, int ybstride)
{
    int pred2 = predicate << 1;
    int value, r, c, d, i, j, l, m, n, count, *lbpcode;
    double *result1, *result2, *D_1, sum;
    
    lbpcode = new int[channel*(columns-pred2)*(rows-pred2)];
    result1 = new double[nbin];
    result2 = new double[nbin];
    D_1 = new double[bits];
    
    fill_n(lbpcode, channel*(columns-pred2)*(rows-pred2), 0);
    
    double *p0 = img,
            *p1 = p0 + predicate,
            *p2 = p1 + predicate,
            *p3 = p2 + columns*predicate,
            *p4 = p3 + columns*predicate,
            *p5 = p4 - predicate,
            *p6 = p5 - predicate,
            *p7 = p6 - columns*predicate,
            *centre = p7 + predicate;
    
    p0 = centre + points[5].x + points[5].y*columns;
    p2 = centre + points[7].x + points[7].y*columns;
    p4 = centre + points[1].x + points[1].y*columns;
    p6 = centre + points[3].x + points[3].y*columns;
    
    for (r=0;r<rows-pred2;++r )
    {
        for (c=0;c<columns-pred2;++c )
        {
            fill_n(D_1, bits, 0.0);
            
            D_1[0]= (int)(interpolate_at_ptr(p0,5,columns)+0.5) - (int)(*centre);
            D_1[1]= (int)(*p1) - (int)(*centre);
            D_1[2]= (int)(interpolate_at_ptr(p2,7,columns)+0.5) - (int)(*centre);
            D_1[3]= (int)(*p3) - (int)(*centre);
            D_1[4]= (int)(interpolate_at_ptr(p4,1,columns)+0.5) - (int)(*centre);
            D_1[5]= (int)(*p5) - (int)(*centre);
            D_1[6]= (int)(interpolate_at_ptr(p6,3,columns)+0.5) - (int)(*centre);
            D_1[7]= (int)(*p7) - (int)(*centre);
            
            value = 0;
            if (D_1[0] >= 0){
                value += 1;
            }
            
            if (D_1[1] >= 0){
                value += 2;
            }
            
            if (D_1[2] >= 0){
                value += 4;
            }
            
            if (D_1[3] >= 0){
                value += 8;
            }
            
            if (D_1[4] >= 0){
                value += 16;
            }
            
            if (D_1[5] >= 0){
                value += 32;
            }
            
            if (D_1[6] >= 0){
                value += 64;
            }
            
            if (D_1[7] >= 0){
                value += 128;
            }
            
            if (value < 128){
                lbpcode[r*(columns-pred2) + c] = (int)map[value];
            }
            else{
                if ((int)map[value] == 0){
                    lbpcode[r*(columns-pred2) + c] = nbin;
                }
                else{
                    lbpcode[r*(columns-pred2) + c] = -(int)map[value];
                }
            }
            
            p0++;
            p1++;
            p2++;
            p3++;
            p4++;
            p5++;
            p6++;
            p7++;
            centre++;
        }
        p0 += pred2;
        p1 += pred2;
        p2 += pred2;
        p3 += pred2;
        p4 += pred2;
        p5 += pred2;
        p6 += pred2;
        p7 += pred2;
        centre += pred2;
    }
    
    p0 = img + columns*rows;
    p1 = p0 + predicate;
    p2 = p1 + predicate;
    p3 = p2 + columns*predicate;
    p4 = p3 + columns*predicate;
    p5 = p4 - predicate;
    p6 = p5 - predicate;
    p7 = p6 - columns*predicate;
    centre = p7 + predicate;
    
    p0 = centre + points[5].x + points[5].y*columns;
    p2 = centre + points[7].x + points[7].y*columns;
    p4 = centre + points[1].x + points[1].y*columns;
    p6 = centre + points[3].x + points[3].y*columns;
    
    for (r=0;r<rows-pred2;++r )
    {
        for (c=0;c<columns-pred2;++c )
        {
            fill_n(D_1, bits, 0.0);
            
            D_1[0]= (int)(interpolate_at_ptr(p0,5,columns)+0.5) - (int)(*centre);
            D_1[1]= (int)(*p1) - (int)(*centre);
            D_1[2]= (int)(interpolate_at_ptr(p2,7,columns)+0.5) - (int)(*centre);
            D_1[3]= (int)(*p3) - (int)(*centre);
            D_1[4]= (int)(interpolate_at_ptr(p4,1,columns)+0.5) - (int)(*centre);
            D_1[5]= (int)(*p5) - (int)(*centre);
            D_1[6]= (int)(interpolate_at_ptr(p6,3,columns)+0.5) - (int)(*centre);
            D_1[7]= (int)(*p7) - (int)(*centre);
            
            value = 0;
            if (D_1[0] >= 0){
                value += 1;
            }
            
            if (D_1[1] >= 0){
                value += 2;
            }
            
            if (D_1[2] >= 0){
                value += 4;
            }
            
            if (D_1[3] >= 0){
                value += 8;
            }
            
            if (D_1[4] >= 0){
                value += 16;
            }
            
            if (D_1[5] >= 0){
                value += 32;
            }
            
            if (D_1[6] >= 0){
                value += 64;
            }
            
            if (D_1[7] >= 0){
                value += 128;
            }
            
            if (value < 128){
                lbpcode[(columns-pred2)*(rows-pred2) + r*(columns-pred2) + c] = (int)map[value];
            }
            else{
                if ((int)map[value] == 0){
                    lbpcode[(columns-pred2)*(rows-pred2) + r*(columns-pred2) + c] = nbin;
                }
                else{
                    lbpcode[(columns-pred2)*(rows-pred2) + r*(columns-pred2) + c] = -(int)map[value];
                }
            }
            
            p0++;
            p1++;
            p2++;
            p3++;
            p4++;
            p5++;
            p6++;
            p7++;
            centre++;
        }
        p0 += pred2;
        p1 += pred2;
        p2 += pred2;
        p3 += pred2;
        p4 += pred2;
        p5 += pred2;
        p6 += pred2;
        p7 += pred2;
        centre += pred2;
    }
    
    p0 = img + 2*columns*rows;
    p1 = p0 + predicate;
    p2 = p1 + predicate;
    p3 = p2 + columns*predicate;
    p4 = p3 + columns*predicate;
    p5 = p4 - predicate;
    p6 = p5 - predicate;
    p7 = p6 - columns*predicate;
    centre = p7 + predicate;
    
    p0 = centre + points[5].x + points[5].y*columns;
    p2 = centre + points[7].x + points[7].y*columns;
    p4 = centre + points[1].x + points[1].y*columns;
    p6 = centre + points[3].x + points[3].y*columns;
    
    for (r=0;r<rows-pred2;++r )
    {
        for (c=0;c<columns-pred2;++c )
        {
            fill_n(D_1, bits, 0.0);
            
            D_1[0]= (int)(interpolate_at_ptr(p0,5,columns)+0.5) - (int)(*centre);
            D_1[1]= (int)(*p1) - (int)(*centre);
            D_1[2]= (int)(interpolate_at_ptr(p2,7,columns)+0.5) - (int)(*centre);
            D_1[3]= (int)(*p3) - (int)(*centre);
            D_1[4]= (int)(interpolate_at_ptr(p4,1,columns)+0.5) - (int)(*centre);
            D_1[5]= (int)(*p5) - (int)(*centre);
            D_1[6]= (int)(interpolate_at_ptr(p6,3,columns)+0.5) - (int)(*centre);
            D_1[7]= (int)(*p7) - (int)(*centre);
            
            value = 0;
            if (D_1[0] >= 0){
                value += 1;
            }
            
            if (D_1[1] >= 0){
                value += 2;
            }
            
            if (D_1[2] >= 0){
                value += 4;
            }
            
            if (D_1[3] >= 0){
                value += 8;
            }
            
            if (D_1[4] >= 0){
                value += 16;
            }
            
            if (D_1[5] >= 0){
                value += 32;
            }
            
            if (D_1[6] >= 0){
                value += 64;
            }
            
            if (D_1[7] >= 0){
                value += 128;
            }
            
            if (value < 128){
                lbpcode[2*(columns-pred2)*(rows-pred2) + r*(columns-pred2) + c] = (int)map[value];
            }
            else{
                if ((int)map[value] == 0){
                    lbpcode[2*(columns-pred2)*(rows-pred2) + r*(columns-pred2) + c] = nbin;
                }
                else{
                    lbpcode[2*(columns-pred2)*(rows-pred2) + r*(columns-pred2) + c] = -(int)map[value];
                }
            }
            
            p0++;
            p1++;
            p2++;
            p3++;
            p4++;
            p5++;
            p6++;
            p7++;
            centre++;
        }
        p0 += pred2;
        p1 += pred2;
        p2 += pred2;
        p3 += pred2;
        p4 += pred2;
        p5 += pred2;
        p6 += pred2;
        p7 += pred2;
        centre += pred2;
    }
    
    count = 0;
    for (i=0;i<=rows-ysize;i+=ybstride){
        for (j=0;j<=columns-xsize;j+=xbstride){
            fill_n(result1, nbin, 0.0);
            fill_n(result2, nbin, 0.0);
            
            for (l=i;l<i+ysize-pred2;++l){
                for (m=j;m<j+xsize-pred2;++m){
                    value = lbpcode[l*(columns-pred2) + m];
                    if (value != nbin){
                        result1[abs(value)]++;
                        if (value > -1){
                            result2[abs(value)]++;
                        }
                        else{
                            result2[abs(value)]--;
                        }
                    }
                    else{
                        result1[0]++;
                        result2[0]--;
                    }
                }
            }
            
            for (l=i;l<i+ysize-pred2;++l){
                for (m=j;m<j+xsize-pred2;++m){
                    value = lbpcode[(columns-pred2)*(rows-pred2) + l*(columns-pred2) + m];
                    if (value != nbin){
                        result1[abs(value)]++;
                        if (value > -1){
                            result2[abs(value)]++;
                        }
                        else{
                            result2[abs(value)]--;
                        }
                    }
                    else{
                        result1[0]++;
                        result2[0]--;
                    }
                }
            }
            
            for (l=i;l<i+ysize-pred2;++l){
                for (m=j;m<j+xsize-pred2;++m){
                    value = lbpcode[2*(columns-pred2)*(rows-pred2) + l*(columns-pred2) + m];
                    if (value != nbin){
                        result1[abs(value)]++;
                        if (value > -1){
                            result2[abs(value)]++;
                        }
                        else{
                            result2[abs(value)]--;
                        }
                    }
                    else{
                        result1[0]++;
                        result2[0]--;
                    }
                }
            }
            
            sum = 0;
            /* L2 Norm */
            /*for (l=nbin;l--;){
             * sum += result[l]*result[l];
             }*/
            for (l=nbin;l--;){
                sum += result1[l] + fabs(result2[l]);
            }
                        
            for (l=nbin;l--; ){
                feature[count*nbin+l] = sqrt(result1[l]);
                feature[(count+1)*nbin+l] = sqrt(fabs(result2[l]));
            }
            
            /* L1 Sqrt Norm */
            sum = sqrt(sum);
            for (l=nbin;l--; ){
                feature[count*nbin+l] = feature[count*nbin+l]/sum;
                feature[(count+1)*nbin+l] = feature[(count+1)*nbin+l]/sum;
            }
            
            count+=2;
        }
    }
    
    if (result1) delete [] result1;
    if (result2) delete [] result2;
    if (lbpcode) delete [] lbpcode;
    if (D_1) delete [] D_1;
}


void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double  *img, *map, *feature;
    int rows, columns, channel, nbin, xsize, ysize, ybstride, xbstride, feature_dim;
    
    /* Check for proper number of arguments */
    
    if (nrhs != 11) {
        mexErrMsgTxt("11 input arguments required!");
    } else if (nlhs > 1) {
        mexErrMsgTxt("Don't need output arguments more than 1!");
    }
    
    calculate_points();
    
    /*Create a pointer to the input matrices*/
    img = mxGetPr(prhs[0]);
    map = mxGetPr(prhs[1]);
    
    /*Get the scalar inputs*/
    rows = (int)mxGetScalar(prhs[2]);
    columns = (int)mxGetScalar(prhs[3]);
    channel = (int)mxGetScalar(prhs[4]);
    feature_dim = (int)mxGetScalar(prhs[5]);
    nbin = (int)mxGetScalar(prhs[6]);
    xsize = (int)mxGetScalar(prhs[7]);
    ysize = (int)mxGetScalar(prhs[8]);
    xbstride = (int)mxGetScalar(prhs[9]);
    ybstride = (int)mxGetScalar(prhs[10]);
    
    plhs[0] = mxCreateDoubleMatrix(feature_dim,1,mxREAL);
    feature = mxGetPr(plhs[0]);
    
    /*Call the C subroutine*/
    lbp_histogram_opt(img,feature,map,rows,columns,channel,nbin,xsize,ysize,xbstride,ybstride);
}