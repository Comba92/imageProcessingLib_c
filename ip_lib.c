/*
 Created by Sebastiano Vascon on 23/03/20.
*/

#include <stdio.h>
#include "ip_lib.h"
#include "bmp.h"

void ip_mat_show(ip_mat * t){
    unsigned int i,l,j;
    printf("Matrix of size %d x %d x %d (hxwxk)\n",t->h,t->w,t->k);
    for (l = 0; l < t->k; l++) {
        printf("Slice %d\n", l);
        for(i=0;i<t->h;i++) {
            for (j = 0; j < t->w; j++) {
                printf("%f ", get_val(t,i,j,l));
            }
            printf("\n");
        }
        printf("\n");
    }
}

void ip_mat_show_stats(ip_mat * t){
    unsigned int k;

    compute_stats(t);

    for(k=0;k<t->k;k++){
        printf("Channel %d:\n", k);
        printf("\t Min: %f\n", t->stat[k].min);
        printf("\t Max: %f\n", t->stat[k].max);
        printf("\t Mean: %f\n", t->stat[k].mean);
    }
}

ip_mat * bitmap_to_ip_mat(Bitmap * img){
    unsigned int i=0,j=0;

    unsigned char R,G,B;

    unsigned int h = img->h;
    unsigned int w = img->w;

    ip_mat * out = ip_mat_create(h, w,3,0);

    for (i = 0; i < h; i++)              /* rows */
    {
        for (j = 0; j < w; j++)          /* columns */
        {
            bm_get_pixel(img, j,i,&R, &G, &B);
            set_val(out,i,j,0,(float) R);
            set_val(out,i,j,1,(float) G);
            set_val(out,i,j,2,(float) B);
        }
    }

    return out;
}

Bitmap * ip_mat_to_bitmap(ip_mat * t){

    Bitmap *b = bm_create(t->w,t->h);

    unsigned int i, j;
    for (i = 0; i < t->h; i++)              /* rows */
    {
        for (j = 0; j < t->w; j++)          /* columns */
        {
            bm_set_pixel(b, j,i, (unsigned char) get_val(t,i,j,0),
                    (unsigned char) get_val(t,i,j,1),
                    (unsigned char) get_val(t,i,j,2));
        }
    }
    return b;
}

float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k){
    if(i<a->h && j<a->w &&k<a->k){  /* j>=0 and k>=0 and i>=0 is non sense*/
        return a->data[i][j][k];
    }else{
        printf("Errore get_val!!!");
        exit(1);
    }
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v){
    if(i<a->h && j<a->w &&k<a->k){
        a->data[i][j][k]=v;
    }else{
        printf("Errore set_val!!!");
        exit(1);
    }
}

float get_normal_random(){
    float y1 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    float y2 = ( (float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    return cos(2*PI*y2)*sqrt(-2.*log(y1));

}

ip_mat * ip_mat_create(unsigned int h, unsigned int w, unsigned int k, float v) {
    ip_mat * m = (ip_mat *) malloc(sizeof(ip_mat));
    m->h = h;
    m->w = w;
    m->k = k;

    m->stat = (stats *) malloc(sizeof(stats));
    m->stat->min = v;
    m->stat->max = v;
    m->stat->mean = v;

    int i,j,l;
    m->data = (float ***) malloc(sizeof(float **) * h);
    for(i=0; i<h; i++) {
        m->data[i] = (float **) malloc(sizeof(float *) * w);
        for(j=0; j<w; j++){
            m->data[i][j] = (float *) malloc(sizeof(float) * k);
            for(l=0; l<k; l++)
                m->data[i][j][l] = v;
        }
    }

    return m;
}

void ip_mat_free(ip_mat *a) {
    free(a->stat);
    
    /* AD OGNI MALLOC UN CORRISPETTIVO FREE!! */
    int i,j;
    for(i=0; i<a->h; i++) {
        for(j=0; j<a->w; j++)
            free(a->data[i][j]);
        free(a->data[i]);
    }
    free(a->data);

    free(a);
}

float get_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k) {
    return a->data[i][j][k];
}

void set_val(ip_mat * a, unsigned int i,unsigned int j,unsigned int k, float v) {
    a->data[i][j][k] = v;
}

void compute_stats(ip_mat * t) {
    /* Max e min definite come macro, sono più efficienti */
    #define max(a,b) (a > b ? a : b);
    #define min(a,b) (a < b ? a : b);
    float minVal, maxVal, sum=0;
    int i,j,k;

    /*Setto al primo valore di data */
    minVal = t->data[0][0][0];
    maxVal = t->data[0][0][0];

    for(i=0; i < t->h; i++)
        for(j=0; j < t->w; j++)
            for(k=0; k < t->k; k++) {
                minVal = min(minVal, t->data[i][j][k]);
                maxVal = max(maxVal, t->data[i][j][k]);
                sum += t->data[i][j][k];
            }
    t->stat->min = minVal;
    t->stat->max = maxVal;
    t->stat->mean = sum / (t->w * t->h * t->k);
}



