#ifndef __LOGOPROPGRAMMEZ_MODEL__H__
#define __LOGOPROPGRAMMEZ_MODEL__H__

#define LOGOPROGRAMMEZ_VERTEX_COUNT 8
const S3L_Unit logoprogrammezVertices[LOGOPROGRAMMEZ_VERTEX_COUNT * 3] = {
  //   X,   Y,    Z     X+ vers la droite, Y+ vers le haut, Z+ vers l'arrière
    -2048,  512, -512,  // face avant   : point en haut à gauche
     2048,  512, -512,  // face avant   : point en haut à droite
     2048, -512, -512,  // face avant   : point en bas à gauche
    -2048, -512, -512,  // face avant   : point en bas à gauche
    -2048,  512, 512,   // face arrière : point en haut à gauche
     2048,  512, 512,   // face arrière : point en haut à droite
     2048, -512, 512,   // face arrière : point en bas à gauche
    -2048, -512, 512    // face arrière : point en bas à gauche
}; // logoprogrammezVertices

#define LOGOPROGRAMMEZ_TRIANGLE_COUNT 12
const S3L_Index logoprogrammezTriangleIndices[LOGOPROGRAMMEZ_TRIANGLE_COUNT * 3] = {
       0,   2,   1, // Programmez
       0,   3,   2, // Décrire les triangles dans le sens inverse des aiguilles d'une montre !

       3,   6,   2, // le magazine
       3,   7,   6,

       7,   5,   6, // des
       7,   4,   5,

       4,   1,   5, // développeurs
       4,   0,   1,


       4,   3,   0, // GB logo
       4,   7,   3,

       1,   6,   5, // C++ logo
       1,   2,   6,

}; // logoprogrammezTriangleIndices

#define LOGOPROGRAMMEZ_UV_COUNT 24
const S3L_Unit logoprogrammezUVs[LOGOPROGRAMMEZ_UV_COUNT * 2] = {
      0,    0,  // "Programmez!" : première ligne de de 1/4 la texture
      512,  0,
      512,  128,
      0,    128,

      0,    128,  // "le magazine" : seconde ligne de 1/4 de la texture
      512,  128,
      512,  256,
      0,    256,

      128,  256,  // "des" : troisème ligne de 1/4 de la texture, sans les quarts gauche et droit (LOGOs).
      384,  256,
      384,  384,
      128,  384,

      0,    384,  // "développeurs" : dernière ligne de 1/4 de la texture
      512,  384,
      512,  512,
      0,    512,

      0,    256,  // GB logo : troisème ligne de 1/4 de la texture, le 1/4 gauche contient le logo Gamebuino
      128,  256,
      128,  384,
      0,    384,

      384,  256,  // C++ logo : troisème ligne de 1/4 de la texture, le 1/4 de droite contient le logo C++
      512,  256,
      512,  384,
      384,  384,

}; // logoprogrammezUVs

#define LOGOPROGRAMMEZ_UV_INDEX_COUNT 12
const S3L_Index logoprogrammezUVIndices[LOGOPROGRAMMEZ_UV_INDEX_COUNT * 3] = {
      0,   2,   1,          // Programmez
      0,   3,   2,

      4+0,   4+2,   4+1,    // Le magazine
      4+0,   4+3,   4+2,

      8+0,   8+2,   8+1,        // des
      8+0,   8+3,   8+2,

      12+0,   12+2,   12+1,     // développeurs
      12+0,   12+3,   12+2,

      16+0,   16+2,   16+1,     // GB logo
      16+0,   16+3,   16+2,

      20+0,   20+2,   20+1,     // C++ logo
      20+0,   20+3,   20+2,


}; // cubetubeUVIndices

S3L_Model3D logoprogrammezModel;
void logoprogrammezModelInit()
{
  S3L_initModel3D(
    logoprogrammezVertices,
    LOGOPROGRAMMEZ_VERTEX_COUNT,
    logoprogrammezTriangleIndices,
    LOGOPROGRAMMEZ_TRIANGLE_COUNT,
    &logoprogrammezModel);
}

#endif // __LOGOPROPGRAMMEZ_MODEL__H__

