#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // permite definir inteiros de tamanho específico 
#include <cmath>
#include <Eigen/Dense>

#pragma pack(push, 1) // diz pro compilador não alterar alinhamento ou tamanho da struct
                         
struct pixel 
{
    uint32_t 
             r:8,
             g:8,
             b:8;
};


struct bmpHeader 
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1,
             reserved2;
    uint32_t offset,
             header_size;
    int32_t  width,
             height;    
    uint16_t planes, 
             bits;
    uint32_t compression,
             imagesize;
    int32_t  xresolution,
             yresolution;
    uint32_t ncolours,
             importantcolours;
    uint32_t redbitmask,greenbitmask,bluebitmask,alphabitmask;
    uint32_t ColorSpaceType;
    uint32_t ColorSpaceEndPoints[9];
    uint32_t Gamma_Red,Gamma_Green,Gamma_Blue,intent,ICCProfileData,ICCProfileSize,Reserved;
    Eigen::Matrix <struct pixel, Eigen::Dynamic, Eigen::Dynamic> image;
};

#pragma pack(pop) // restaura comportamento do compilador 

typedef Eigen::Matrix <struct pixel, Eigen::Dynamic, Eigen::Dynamic> MatrixIMG;

void printbin(char texto[], unsigned int valor)
{
     int i;
     printf("%s",texto);
     for (i=31;i>=0;i--) printf("%d",valor >> i & 1); 
     printf("\n");
}

int main() 
{
    FILE *imagemI;
    FILE *imagemGS;
    FILE *imagemFDS;

    imagemI = fopen("./IMGS/IMG_Classica.BMP", "rb");  
    imagemGS = fopen("./IMGS/IMG_ClassicaGS.BMP", "wb");
    imagemFDS = fopen("./IMGS/IMG_ClassicaFDS.BMP", "wb");

    int kernelx [3][3] = {1,0,-1,
                          2,0,-2,
                          1,0,-1};
    int kernely [3][3] = {1,2,1,
                          0,0,0,
                         -1,-2,-1};                  
    struct bmpHeader bH;

    fread(&bH, sizeof(struct bmpHeader), 1, imagemI);
    fwrite(&bH, sizeof(struct bmpHeader), 1, imagemGS);
    fwrite(&bH, sizeof(struct bmpHeader), 1, imagemFDS);

    float matrizmedia[bH.height][bH.width];

    MatrixIMG matrizI;
    MatrixIMG matrizGS;
    MatrixIMG matrizFDS;


    matrizI.resize(bH.height,bH.width);
    matrizGS.resize(bH.height,bH.width);
    matrizFDS.resize(bH.height,bH.width);

    //matriz de leitura;

    int x, y;

    // leitura 
    for (x = 0; x < bH.height; x++) 
    {
        for (y = 0; y < bH.width; y++) 
        {
            fread (&(matrizI(x,y)), sizeof(struct pixel), 1, imagemI);
        }
    }

    // greyscale

    for (x = 0; x < bH.height; x++) 
    {
        for (y = 0; y < bH.width; y++) 
        {
            int media = ((matrizI(x,y).r) + (matrizI(x,y).g)  + (matrizI(x,y).b))/3;
            matrizGS(x,y).g = media;
            matrizGS(x,y).r = media;
            matrizGS(x,y).b = media;
        }
    }
    

    //sobel
    float sumxr=0;
    float sumyr=0;
    float sumxg=0;
    float sumyg=0;
    float sumxb=0;
    float sumyb=0;

    for (x = 0; x < bH.height; x++) 
    {
        for (y = 0; y < bH.width; y++) 
        {
            sumxr=0;
            sumyr=0;
            sumxg=0;
            sumyg=0;
            sumxb=0;
            sumyb=0;
            for (int xk = 0; xk < 2; xk++) 
            {
                for (int yk = 0; yk < 2; yk++) 
                {
                    sumxr = sumxr += ((matrizGS(x,y).r)*(kernelx[xk][yk]));
                    sumyr = sumyr += ((matrizGS(x,y).r)*(kernely[xk][yk]));

                    sumxg = sumxg += ((matrizGS(x,y).g)*(kernelx[xk][yk]));
                    sumyg = sumyg += ((matrizGS(x,y).g)*(kernely[xk][yk]));

                    sumxb = sumxb += ((matrizGS(x,y).b)*(kernelx[xk][yk]));
                    sumyb = sumyb += ((matrizGS(x,y).b)*(kernely[xk][yk]));
                }
            }
            
            matrizFDS(x,y).r = (int)(sqrt((sumxr*sumxr)+(sumyr*sumyr)));
            matrizFDS(x,y).g = (int)(sqrt((sumxg*sumxg)+(sumyg*sumyg)));
            matrizFDS(x,y).b = (int)(sqrt((sumxb*sumxb)+(sumyb*sumyb)));



            //matrizFDS[x][y].r = atan(sumy/sumx);
            //matrizFDS[x][y].g = atan(sumy/sumx);
            //matrizFDS[x][y].b = atan(sumy/sumx);
            if ((matrizFDS(x,y).r >(128))|| (matrizFDS(x,y).g >(128))|| (matrizFDS(x,y).b >(128)))
            {
                matrizFDS(x,y).r = 255;
                matrizFDS(x,y).g = 255;
                matrizFDS(x,y).b = 255;
            }
            else
            {
                matrizFDS(x,y).r = 0;
                matrizFDS(x,y).g = 0;
                matrizFDS(x,y).b = 0; 
            }
        }
    }
    
    // escrita greyscale
    for (x = 0; x < bH.height; x++) 
    {
        for (y = 0; y < bH.width; y++) 
        {
            fwrite(&matrizGS(x,y), sizeof(pixel), 1, imagemGS);
        }
        
    }

    // escrita filtro 
    for (x = 0; x < bH.height; x++) 
    {
        for (y = 0; y < bH.width; y++) 
        {
            fwrite(&matrizFDS(x,y), sizeof(pixel), 1, imagemFDS);
        }
        
    }
    fclose(imagemGS);
    fclose(imagemFDS);
    return 0;
}