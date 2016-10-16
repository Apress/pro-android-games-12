/*
Copyright (C) 2007-2009 Olli Hinkka

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qgl.h"
//#include "glesinterface.h"
#include <GLES/gl.h>

#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1

struct nanoState                 
    {                                  
    GLboolean alpha_test;              
    GLboolean blend;                   
    GLboolean clip_planei;             
    GLboolean color_logic_op;          
    GLboolean color_material;          
    GLboolean cull_face;               
    GLboolean depth_test;              
    GLboolean dither;                  
    GLboolean fog;                     
    GLboolean lighti;                  
    GLboolean lighting;                
    GLboolean line_smooth;             
    GLboolean matrix_palette_oes;      
    GLboolean multisample;             
    GLboolean normalize;               
    GLboolean point_smooth;            
    GLboolean point_sprite_oes;        
    GLboolean polygon_offset_fill;     
    GLboolean rescale_normal;          
    GLboolean sample_alpha_to_coverage;
    GLboolean sample_alpha_to_one;     
    GLboolean sample_coverage;         
    GLboolean scissor_test;            
    GLboolean stencil_test;            
    GLboolean depthmask;               
    GLclampf depth_range_near;         
    GLclampf depth_range_far;          
    GLenum depth_func;                 
    GLenum cullface;                   
    GLenum shademodel;                 
    GLenum sfactor;                    
    GLenum dfactor;                    
    GLenum matrixmode;                 
    };                                 

static struct nanoState nanoqglState;

static struct nanoState nanoqglInitState = 
    {                                
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_TRUE,                         
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_TRUE,                         
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_TRUE,                         
    0.0f,                            
    1.0f,                            
    GL_LESS,                         
    GL_BACK,                         
    GL_SMOOTH,                       
    GL_ONE,                          
    GL_ZERO,                         
    GL_MODELVIEW,                                        
    };                               

struct booleanstate
    {
    GLboolean value;
    GLboolean changed;
    };

struct floatstate
    {
    GLfloat value;
    GLboolean changed;
    };
    
struct uintstate
    {
    GLuint value;
    GLboolean changed;
    };
    
struct ptrstate
    {
    GLint size;
    GLenum type;
    GLsizei stride;
    GLvoid* ptr;
    GLboolean changed;
    GLboolean enabled;
    };


struct nanotmuState
    {
    struct booleanstate texture_2d;
    struct floatstate texture_env_mode;                                         
    struct uintstate boundtexture;      
    struct ptrstate vertex_array;                                       
    struct ptrstate color_array;
    struct ptrstate texture_coord_array;
    };

static struct nanotmuState tmuState0;
static struct nanotmuState tmuState1;
        
static struct nanotmuState tmuInitState = 
    {
    {GL_FALSE, GL_FALSE},           
    {GL_MODULATE,GL_FALSE},        
    {0x7fffffff,GL_FALSE},
    {4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
    {4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
    {4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
    };   

static struct nanotmuState* activetmuState = &tmuState0;

//extern "C++" GlESInterface* glEsImpl;

static GLenum wrapperPrimitiveMode = GL_QUADS;
GLboolean useTexCoordArray = GL_FALSE;
static GLenum activetmu = GL_TEXTURE0;
static GLenum clientactivetmu = GL_TEXTURE0;

#if defined(__MULTITEXTURE_SUPPORT__)
GLboolean useMultiTexCoordArray = GL_FALSE;
#endif


#if !defined (__WINS__)
//#define __FORCEINLINE __forceinline
#define __FORCEINLINE inline
#else
#define __FORCEINLINE
#endif

static GLboolean delayedttmuchange = GL_FALSE;
static GLenum delayedtmutarget = GL_TEXTURE0;

struct VertexAttrib
    {
    float x;
    float y;
    float z;
#if !defined(__MULTITEXTURE_SUPPORT__)
    float padding;
#endif
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    
    float s;
    float t;
	float r; // Vladimir
#if defined(__MULTITEXTURE_SUPPORT__)
    float s_multi;
    float t_multi;
#endif
    };

static VertexAttrib vertexattribs[8000];

static GLushort indexArray[30000];

static GLuint vertexCount = 0;
static GLuint indexCount = 0;
static GLuint vertexMark = 0;
static int indexbase = 0;

static VertexAttrib* ptrVertexAttribArray = NULL;
static VertexAttrib* ptrVertexAttribArrayMark = NULL;

static VertexAttrib currentVertexAttrib;
#if defined(__MULTITEXTURE_SUPPORT__)
static VertexAttrib currentVertexAttribInit = {0.0f,0.0f,0.0f,
                                    255,255,255,255,
                                    0.0f,0.0f,
                                    0.0f,0.0f
                                    };
#else
static VertexAttrib currentVertexAttribInit = {0.0f,0.0f,0.0f,0.0f,
                                    255,255,255,255,
                                    0.0f,0.0f,
                                    };
#endif
static GLushort* ptrIndexArray = NULL;

static GLboolean arraysValid = GL_FALSE;

void InitGLStructs()
    {
    ptrVertexAttribArray = vertexattribs;
    ptrVertexAttribArrayMark = ptrVertexAttribArray;
    ptrIndexArray = indexArray;
    
    memcpy(&nanoqglState, &nanoqglInitState, sizeof(struct nanoState));
    memcpy(&tmuState0,&tmuInitState,sizeof(struct nanotmuState));
    memcpy(&tmuState1,&tmuInitState,sizeof(struct nanotmuState));    
    memcpy(&currentVertexAttrib,&currentVertexAttribInit,sizeof(struct VertexAttrib));
    
    activetmuState = &tmuState0;
    wrapperPrimitiveMode = GL_QUADS;
    useTexCoordArray = GL_FALSE;
    activetmu = GL_TEXTURE0;
    clientactivetmu = GL_TEXTURE0;
    delayedttmuchange = GL_FALSE;
    delayedtmutarget = GL_TEXTURE0;
    vertexCount = 0;
    indexCount = 0;
    vertexMark = 0;
    indexbase = 0;
    arraysValid = GL_FALSE;
    }


void FlushOnStateChange()
    {
    if (delayedttmuchange)
        {
        delayedttmuchange = GL_FALSE;
        glActiveTexture(delayedtmutarget);
        }

    if (!vertexCount)
        return;

    if (!arraysValid)
        {
        glClientActiveTexture(GL_TEXTURE0);
        glVertexPointer(3, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexAttrib), &vertexattribs[0].red);
        // Vladimir glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s);    
		glTexCoordPointer(3, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s);    
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
#if defined(__MULTITEXTURE_SUPPORT__)        
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s_multi); 
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);  
        glClientActiveTexture(GL_TEXTURE0);   
#endif
        arraysValid = GL_TRUE;
        }

    glDrawElements( GL_TRIANGLES,vertexCount,GL_UNSIGNED_SHORT, indexArray );


#if defined(__MULTITEXTURE_SUPPORT__)            
    useMultiTexCoordArray = GL_FALSE;
#endif
    vertexCount = 0;
    indexCount = 0;
    ptrVertexAttribArray = vertexattribs;
    ptrVertexAttribArrayMark = ptrVertexAttribArray;
    ptrIndexArray = indexArray;
    useTexCoordArray = GL_FALSE;
    }

void qglBegin(GLenum mode)
    {
    wrapperPrimitiveMode = mode;
    vertexMark = vertexCount;    
    ptrVertexAttribArrayMark = ptrVertexAttribArray;
    indexbase = indexCount;
    }
    
void qglEnd(void)
    {
    vertexCount+=((unsigned char*)ptrVertexAttribArray-(unsigned char*)ptrVertexAttribArrayMark)/sizeof(VertexAttrib);
    if (vertexCount < 3)
        {
        return;
        }
    switch (wrapperPrimitiveMode)
        {
        case GL_QUADS:
            {
            *ptrIndexArray++ = indexCount;
            *ptrIndexArray++ = indexCount+1;
            *ptrIndexArray++ = indexCount+2;
            *ptrIndexArray++ = indexCount;
            *ptrIndexArray++ = indexCount+2;
            *ptrIndexArray++ = indexCount+3;
            indexCount+=4;
            vertexCount+=2;
            }
            break;
        case GL_TRIANGLES:
            {
            int  vcount = (vertexCount-vertexMark)/3;
            for (int count = 0; count < vcount; count++)
                {
                *ptrIndexArray++ = indexCount;
                *ptrIndexArray++ = indexCount+1;
                *ptrIndexArray++ = indexCount+2;            
                indexCount+=3;    
                }
            }
            break;
        case GL_TRIANGLE_STRIP:
            {
            *ptrIndexArray++ = indexCount;
            *ptrIndexArray++ = indexCount+1;
            *ptrIndexArray++ = indexCount+2;
            indexCount+=3;
            int vcount = ((vertexCount-vertexMark)-3);
            if (vcount && ((long)ptrIndexArray & 0x02))
                {
                *ptrIndexArray++ = indexCount-1; // 2 
                *ptrIndexArray++ = indexCount-2; // 1
                *ptrIndexArray++ = indexCount;   // 3
                indexCount++;
                vcount-=1;    
                int odd = vcount&1;
                vcount/=2;
                unsigned int* longptr = (unsigned int*) ptrIndexArray;

                for (int count = 0; count < vcount; count++)
                    {
                    *(longptr++) = (indexCount-2) | ((indexCount-1)<<16);                    
                    *(longptr++) = (indexCount) | ((indexCount)<<16);                    
                    *(longptr++) = (indexCount-1) | ((indexCount+1)<<16);
                    indexCount+=2;
                    }
                ptrIndexArray = (unsigned short*)(longptr);
                if (odd)
                    {
                    *ptrIndexArray++ = indexCount-2; // 2 
                    *ptrIndexArray++ = indexCount-1; // 1
                    *ptrIndexArray++ = indexCount;   // 3
                    indexCount++;    
                    }
                }
           else
                {
                //already aligned
                int odd = vcount&1;
                vcount/=2;
                unsigned int* longptr = (unsigned int*) ptrIndexArray;

                for (int count = 0; count < vcount; count++)
                    {                    
                    *(longptr++) = (indexCount-1) | ((indexCount-2)<<16);                    
                    *(longptr++) = (indexCount) | ((indexCount-1)<<16);                    
                    *(longptr++) = (indexCount) | ((indexCount+1)<<16);
                    indexCount+=2;

                    }
                ptrIndexArray = (unsigned short*)(longptr);
                if (odd)
                    {
                    
                    *ptrIndexArray++ = indexCount-1; // 2 
                    *ptrIndexArray++ = indexCount-2; // 1
                    *ptrIndexArray++ = indexCount;   // 3
                    indexCount++;    
                    }
                }            
            vertexCount+=(vertexCount-vertexMark-3)*2;
            }
            break;
        case GL_POLYGON:
        case GL_TRIANGLE_FAN:
            {
            *ptrIndexArray++ = indexCount++;
            *ptrIndexArray++ = indexCount++;
            *ptrIndexArray++ = indexCount++;
            int vcount = ((vertexCount-vertexMark)-3);
            for (int count = 0; count < vcount; count++)
                {
                *ptrIndexArray++ = indexbase;
                *ptrIndexArray++ = indexCount-1;
                *ptrIndexArray++ = indexCount++;                
                vertexCount+=2;
                }
             }   
            break;

        default:
            break;
        }
    }

void qglEnable (GLenum cap)
    {
    GLboolean statechanged = GL_FALSE;
    switch(cap)
        {
        case GL_ALPHA_TEST:
            {
            if (!nanoqglState.alpha_test)
                {
                nanoqglState.alpha_test = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_BLEND:
            {
            if (!nanoqglState.blend)
                {
                nanoqglState.blend = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        //case GL_CLIP_PLANEi
        case GL_COLOR_LOGIC_OP:
            {
            if (!nanoqglState.color_logic_op)
                {
                nanoqglState.color_logic_op = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_COLOR_MATERIAL:
            {
            if (!nanoqglState.color_material)
                {
                nanoqglState.color_material = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_CULL_FACE:
            {
            if (!nanoqglState.cull_face)
                {
                nanoqglState.cull_face = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DEPTH_TEST:
            {
            if (!nanoqglState.depth_test)
                {
                nanoqglState.depth_test = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DITHER:
            {
            if (!nanoqglState.dither)
                {
                nanoqglState.dither = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_FOG:
        //case GL_LIGHTi
            {
            if (!nanoqglState.fog)
                {
                nanoqglState.fog = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LIGHTING:
            {
            if (!nanoqglState.lighting)
                {
                nanoqglState.lighting = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LINE_SMOOTH:
            {
            if (!nanoqglState.line_smooth)
                {
                nanoqglState.line_smooth = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_MATRIX_PALETTE_OES:
            {
            if (!nanoqglState.matrix_palette_oes)
                {
                nanoqglState.matrix_palette_oes = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_MULTISAMPLE:
            {
            if (!nanoqglState.multisample)
                {
                nanoqglState.multisample = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_NORMALIZE:
            {
            if (!nanoqglState.normalize)
                {
                nanoqglState.normalize = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_POINT_SPRITE_OES:
            {
            if (!nanoqglState.point_sprite_oes)
                {
                nanoqglState.point_sprite_oes = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_POLYGON_OFFSET_FILL:
            {
            if (!nanoqglState.polygon_offset_fill)
                {
                nanoqglState.polygon_offset_fill = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_RESCALE_NORMAL:
            {
            if (!nanoqglState.rescale_normal)
                {
                nanoqglState.rescale_normal = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_COVERAGE:
            {
            if (!nanoqglState.sample_alpha_to_coverage)
                {
                nanoqglState.sample_alpha_to_coverage = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_ONE:
            {
            if (!nanoqglState.sample_alpha_to_one)
                {
                nanoqglState.sample_alpha_to_one = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_COVERAGE:
            {
            if (!nanoqglState.sample_coverage)
                {
                nanoqglState.sample_coverage = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SCISSOR_TEST:
            {
            if (!nanoqglState.scissor_test)
                {
                nanoqglState.scissor_test = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_STENCIL_TEST:
            {
            return;
/*            if (!nanoqglState.stencil_test)
                {
                nanoqglState.stencil_test = GL_TRUE;
                statechanged = GL_TRUE;
                }*/
            break;
            }
        case GL_TEXTURE_2D:   
            {
            if (!activetmuState->texture_2d.value)
                {
                FlushOnStateChange();
                glEnable(cap);
                activetmuState->texture_2d.value = GL_TRUE;
                return;              
                }
            break;
            }
        default:
            break;
        }
        
    if (statechanged)
        {
        FlushOnStateChange();
        glEnable(cap);
        }    
    }

void qglDisable (GLenum cap)
    {
    GLboolean statechanged = GL_FALSE;
    switch(cap)
        {
        case GL_ALPHA_TEST:
            {
            if (nanoqglState.alpha_test)
                {
                nanoqglState.alpha_test = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_BLEND:
            {
            if (nanoqglState.blend)
                {
                nanoqglState.blend = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        //case GL_CLIP_PLANEi
        case GL_COLOR_LOGIC_OP:
            {
            if (nanoqglState.color_logic_op)
                {
                nanoqglState.color_logic_op = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_COLOR_MATERIAL:
            {
            if (nanoqglState.color_material)
                {
                nanoqglState.color_material = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_CULL_FACE:
            {
            if (nanoqglState.cull_face)
                {
                nanoqglState.cull_face = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DEPTH_TEST:
            {
            if (nanoqglState.depth_test)
                {
                nanoqglState.depth_test = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DITHER:
            {
            if (nanoqglState.dither)
                {
                nanoqglState.dither = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_FOG:
        //case GL_LIGHTi
            {
            if (nanoqglState.fog)
                {
                nanoqglState.fog = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LIGHTING:
            {
            if (nanoqglState.lighting)
                {
                nanoqglState.lighting = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LINE_SMOOTH:
            {
            if (nanoqglState.line_smooth)
                {
                nanoqglState.line_smooth = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_MATRIX_PALETTE_OES:
            {
            if (nanoqglState.matrix_palette_oes)
                {
                nanoqglState.matrix_palette_oes = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_MULTISAMPLE:
            {
            if (nanoqglState.multisample)
                {
                nanoqglState.multisample = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_NORMALIZE:
            {
            if (nanoqglState.normalize)
                {
                nanoqglState.normalize = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_POINT_SPRITE_OES:
            {
            if (nanoqglState.point_sprite_oes)
                {
                nanoqglState.point_sprite_oes = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_POLYGON_OFFSET_FILL:
            {
            if (nanoqglState.polygon_offset_fill)
                {
                nanoqglState.polygon_offset_fill = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_RESCALE_NORMAL:
            {
            if (nanoqglState.rescale_normal)
                {
                nanoqglState.rescale_normal = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_COVERAGE:
            {
            if (nanoqglState.sample_alpha_to_coverage)
                {
                nanoqglState.sample_alpha_to_coverage = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_ONE:
            {
            if (nanoqglState.sample_alpha_to_one)
                {
                nanoqglState.sample_alpha_to_one = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_COVERAGE:
            {
            if (nanoqglState.sample_coverage)
                {
                nanoqglState.sample_coverage = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SCISSOR_TEST:
            {
            if (nanoqglState.scissor_test)
                {
                nanoqglState.scissor_test = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_STENCIL_TEST:
            {
            return;
/*            if (nanoqglState.stencil_test)
                {
                nanoqglState.stencil_test = GL_FALSE;
                statechanged = GL_TRUE;
                }*/
            break;
            }
        case GL_TEXTURE_2D:   
            {
            if (activetmuState->texture_2d.value)
                {
                FlushOnStateChange();
                glDisable(cap);
                activetmuState->texture_2d.value = GL_FALSE;
                return;


                }
            break;
            }
        default:
            break;
        }
        
    if (statechanged)
        {
        FlushOnStateChange();
        glDisable(cap);
        }    
    }
    
void qglVertex2f(GLfloat x, GLfloat y)
    {
    qglVertex3f(x,y,0.0f);
    }


__FORCEINLINE unsigned int ClampTo255(float value)
    {
    unsigned int retval = (unsigned int)(value);
    if (retval > 255)
        {
        retval = 255;
        }
    return retval;
    }

void qglColor3f(	GLfloat red, GLfloat green, GLfloat blue)
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(red*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(green*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(blue*255.0f);
    currentVertexAttrib.alpha = 255;
    }

void qglTexCoord2fv( const GLfloat *v ) 
    {
    memcpy(&currentVertexAttrib.s, v, 2*sizeof(float));
    }


void qglTexCoord2f(GLfloat s, GLfloat t)
    {
    currentVertexAttrib.s = s;
    currentVertexAttrib.t = t;
	currentVertexAttrib.r = 0; // Vladimir
    }

// Vladimir	
void qglTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
    {
    currentVertexAttrib.s = s;
    currentVertexAttrib.t = t;
	currentVertexAttrib.r = r;
    }
    
void qglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
    {
    FlushOnStateChange();
    glViewport(x,y,width,height);
    }
    
void qglLoadIdentity (void)
    {
    FlushOnStateChange();
    glLoadIdentity();
    }
    
void qglColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(red*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(green*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(blue*255.0f);
    currentVertexAttrib.alpha = (unsigned char)ClampTo255(alpha*255.0f);
    }

void qglOrtho (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
    {
    FlushOnStateChange();
    glOrthof(left,right,bottom,top, zNear,zFar);
    }

void qglMatrixMode (GLenum mode)
    {
    if (nanoqglState.matrixmode == mode)
        {
        return;
        }
    nanoqglState.matrixmode = mode;
    FlushOnStateChange();
    glMatrixMode(mode);
    }

void qglTexParameterf (GLenum target, GLenum pname, GLfloat param)
    {
    if (pname == GL_TEXTURE_BORDER_COLOR)
        {
        return; // not supported by openqgl es
        }
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)
             {
             param = 0x812F;
             }

    FlushOnStateChange();
    glTexParameterf(target, pname,param);
    }
    
void qglTexParameterfv(	GLenum target, GLenum pname, const GLfloat *params) 
    {
    qglTexParameterf(target, pname, params[0]);
    }

void qglTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
    {
    FlushOnStateChange();
    internalformat = format;
    glTexImage2D(target, level, internalformat, width, height,border,format,type,pixels);
    }

void qglDrawBuffer(GLenum /*mode*/)
    {
    }

void qglTranslatef (GLfloat x, GLfloat y, GLfloat z)
    {
    FlushOnStateChange();
    glTranslatef(x,y,z);
    }
    
void qglRotatef (GLfloat anqgle, GLfloat x, GLfloat y, GLfloat z)
    {
    FlushOnStateChange();
    glRotatef(anqgle, x, y, z);
    }

void qglScalef (GLfloat x, GLfloat y, GLfloat z)
    {
    FlushOnStateChange();
    glScalef(x,y,z);
    }
    
void qglDepthRange(GLclampf zNear, GLclampf zFar)
    {
    if ((nanoqglState.depth_range_near == zNear) &&(nanoqglState.depth_range_far == zFar))
        {
        return;
        }
    else
        {
        nanoqglState.depth_range_near = zNear;
        nanoqglState.depth_range_far = zFar;
        }
    FlushOnStateChange();
    glDepthRangef(zNear, zFar);
    }

void qglDepthFunc (GLenum func)
    {
    if (nanoqglState.depth_func == func)
        {
        return;
        }
    else
        {
        nanoqglState.depth_func = func;
        }
    FlushOnStateChange();
    glDepthFunc(func);
    }

void qglFinish (void)
    {
    FlushOnStateChange();
    glFinish();
    }

void qglGetFloatv (GLenum pname, GLfloat *params)
    {
    FlushOnStateChange();
    glGetFloatv(pname, params);
    }

void qglCullFace (GLenum mode)
    {
    if (nanoqglState.cullface == mode)
        {
        return;
        }
    else
        {
        nanoqglState.cullface = mode;
        }
    FlushOnStateChange();
    glCullFace(mode);
    }

void qglFrustum (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
    {
    FlushOnStateChange();
    glFrustumf(left,right,bottom,top,zNear,zFar);
    }

void qglClear (GLbitfield mask)
    {
    FlushOnStateChange();
    glClear(mask);
    }


void qglVertex3f( GLfloat x, GLfloat y, GLfloat z )
    {
    GLfloat* vert = (GLfloat*)ptrVertexAttribArray++;
    *vert++ = x;
    *vert++ = y;
    *vert++ = z;
#if defined(__MULTITEXTURE_SUPPORT__)
    memcpy(vert, &currentVertexAttrib.red, 5*sizeof(GLfloat));
#else
    memcpy(vert+1, &currentVertexAttrib.red, 3*sizeof(GLfloat));
#endif

    }

void qglColor4fv( const GLfloat *v )
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(v[0]*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(v[1]*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(v[2]*255.0f);
    currentVertexAttrib.alpha = (unsigned char)ClampTo255(v[3]*255.0f);
    }
    
void qglColor3ubv( const GLubyte* v)
    {
    currentVertexAttrib.red = v[0];
    currentVertexAttrib.green = v[1];
    currentVertexAttrib.blue = v[2];
    currentVertexAttrib.alpha = 255;
    }
    
void qglColor4ubv( const GLubyte *v ) 
    {  
    *((unsigned int*)(&currentVertexAttrib.red)) = *((unsigned int*)(v));
    }
    
void qglColor3fv( const GLfloat *v ) 
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(v[0]*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(v[1]*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(v[2]*255.0f);
    currentVertexAttrib.alpha = 255;
    }



void qglHint (GLenum target, GLenum mode)
    {
    FlushOnStateChange();
    glHint(target, mode);
    }

void qglBlendFunc (GLenum sfactor, GLenum dfactor)
    {
    if ((nanoqglState.sfactor == sfactor) && (nanoqglState.dfactor == dfactor))
        {
        return;
        }
    nanoqglState.sfactor = sfactor;
    nanoqglState.dfactor = dfactor;
    FlushOnStateChange();
    glBlendFunc(sfactor, dfactor);
    }

void qglPopMatrix (void)
    {
    FlushOnStateChange();
    glPopMatrix();
    }

void qglShadeModel (GLenum mode)
    {
    if (nanoqglState.shademodel == mode)
        {
        return;
        }
    nanoqglState.shademodel = mode;
    FlushOnStateChange();
    glShadeModel(mode);
    }

void qglPushMatrix (void)
    {
    FlushOnStateChange();
    glPushMatrix();
    }

void qglTexEnvf (GLenum target, GLenum pname, GLfloat param)
    {
    if (target == GL_TEXTURE_ENV)
        {
        if (pname == GL_TEXTURE_ENV_MODE)
            {
            if (param == activetmuState->texture_env_mode.value)
                {
                return;
                } 
            else
                {
                FlushOnStateChange();
                glTexEnvf(target, pname, param);
                activetmuState->texture_env_mode.value = param;
                return;
                }
            }
        }
    FlushOnStateChange();
    glTexEnvf(target, pname, param);
    }

void qglVertex3fv( const GLfloat *v )
    {
    GLfloat* vert = (GLfloat*)ptrVertexAttribArray++;
    memcpy(vert, v, 3*sizeof(GLfloat));
#if defined(__MULTITEXTURE_SUPPORT__)
    memcpy(vert+3, &currentVertexAttrib.red, 5*sizeof(GLfloat));
#else
    memcpy(vert+4, &currentVertexAttrib.red, 3*sizeof(GLfloat));
#endif
    }
    

void qglDepthMask (GLboolean flag)
    {
    if (nanoqglState.depthmask == flag)
        {
        return;
        }
    nanoqglState.depthmask = flag;
    FlushOnStateChange();
    glDepthMask(flag);
    }

void qglBindTexture (GLenum target, GLuint texture)
    {
    if (activetmuState->boundtexture.value == texture)
        {
        return;
        }
    FlushOnStateChange();
    activetmuState->boundtexture.value = texture;
    glBindTexture(target, texture);
    }


void qglGetIntegerv (GLenum pname, GLint *params)
    {
    FlushOnStateChange();
    glGetIntegerv(pname, params);
    }

GLubyte nano_extensions_string[4096];
const GLubyte* qglGetString (GLenum name)
    {

    if (name == GL_EXTENSIONS)
        {
#if defined(__MULTITEXTURE_SUPPORT__)
        sprintf((char*)nano_extensions_string,"%s %s",glGetString(name),"GL_ARB_multitexture EXT_texture_env_add");
#else
        sprintf((char*)nano_extensions_string,"%s %s",glGetString(name),"EXT_texture_env_add");
#endif
        return nano_extensions_string;
        }
    return glGetString(name);
    }

void qglAlphaFunc (GLenum func, GLclampf ref)
    {
    FlushOnStateChange();
    glAlphaFunc(func,ref);
    }

void qglFlush (void)
    {
    FlushOnStateChange();
    glFlush();
    }

void qglReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
    {
    if (format == GL_DEPTH_COMPONENT)
        {
        // OpenqglEs 1.1 does not support reading depth buffer without an extension
        memset(pixels, 0xff,4);
        return;   
        }
    FlushOnStateChange();
    glReadPixels(x,y,width,height,format,type,pixels);
    }

void qglReadBuffer( GLenum /*mode*/ )
    {
    
    }

void qglLoadMatrixf (const GLfloat *m)
    {
    FlushOnStateChange();
    glLoadMatrixf(m);
    }

void qglTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
    {
    FlushOnStateChange();
    glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
    }

void qglClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
    {
    FlushOnStateChange();
    glClearColor(red,green,blue,alpha);
    }

GLenum qglGetError (void)
    {
    FlushOnStateChange();
    return GL_NO_ERROR;//glGetError();
    }

void qglActiveTexture (GLenum texture)
    {
    if (activetmu == texture)
        {
        return;
        }
    if (delayedttmuchange)
        {
        delayedttmuchange = GL_FALSE;
        }
    else
        {
        delayedttmuchange = GL_TRUE;
        delayedtmutarget = texture;
        }
    if (texture == GL_TEXTURE0)
        {
        activetmuState = &tmuState0;
        }
    else
        {
        activetmuState = &tmuState1;
        }
    activetmu = texture;
    }
    
void qglClientActiveTexture (GLenum texture)
    {
    clientactivetmu = texture;
    }
    
void qglPolygonMode( GLenum face, GLenum mode )
    {
    }

void qglDeleteTextures( GLsizei n, const GLuint *textures ) 
    {
    FlushOnStateChange();
    glDeleteTextures(n,textures);
    }

void qglClearDepth( GLclampf depth ) 
    {
    FlushOnStateChange();
    glClearDepthf( depth ); 
    }

void qglClipPlane( GLenum plane, const GLdouble *equation ) 
    {
    FlushOnStateChange();
    float array[4];
    array[0] = (GLfloat)(equation[0]);
    array[1] = (GLfloat)(equation[1]);
    array[2] = (GLfloat)(equation[2]);
    array[3] = (GLfloat)(equation[3]);
    glClipPlanef( plane, array );
    }

void qglScissor( GLint x, GLint	y, GLsizei width, GLsizei height ) 
    {
    FlushOnStateChange();
    glScissor( x, y, width,height);
    }

void qglPointSize( GLfloat size ) 
    {
    FlushOnStateChange();
    glPointSize( size );
    }

void qglArrayElement(GLint i) {}
void qglLineWidth(GLfloat width) {}
//void qglCallList( GLuint list ) {}
void qglColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {}
void qglStencilFunc( GLenum func, GLint ref, GLuint mask ) {}
void qglStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {}

struct ptrstate vertex_array;                                       
struct ptrstate color_array;
struct ptrstate texture_coord_array;

void qglDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) 
    {
    // ensure that all primitives specified between qglBegin/qglEnd pairs
    // are rendered first, and that we have correct tmu in use..
    FlushOnStateChange();
    // setup correct vertex/color/texcoord pointers
    if (arraysValid || 
        tmuState0.vertex_array.changed ||
        tmuState0.color_array.changed ||
        tmuState0.texture_coord_array.changed)
        {
        glClientActiveTexture(GL_TEXTURE0);
        }
    if (arraysValid || tmuState0.vertex_array.changed)
        {
        if (tmuState0.vertex_array.enabled)
            {
            glEnableClientState(GL_VERTEX_ARRAY);
            }
        else
            {
            glDisableClientState(GL_VERTEX_ARRAY);
            }
        glVertexPointer(tmuState0.vertex_array.size, 
                                  tmuState0.vertex_array.type, 
                                  tmuState0.vertex_array.stride, 
                                  tmuState0.vertex_array.ptr);
        tmuState0.vertex_array.changed = GL_FALSE;
        }
    if (arraysValid || tmuState0.color_array.changed)
        {
        if (tmuState0.color_array.enabled)
            {
            glEnableClientState(GL_COLOR_ARRAY);
            }
        else
            {
            glDisableClientState(GL_COLOR_ARRAY);
            }
        glColorPointer(tmuState0.color_array.size, 
                                 tmuState0.color_array.type, 
                                 tmuState0.color_array.stride, 
                                 tmuState0.color_array.ptr);
        tmuState0.color_array.changed = GL_FALSE;
        }
    if (arraysValid || tmuState0.texture_coord_array.changed)
        {
        tmuState0.texture_coord_array.changed = GL_FALSE;
        if (tmuState0.texture_coord_array.enabled)
            {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        else
            {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        glTexCoordPointer(tmuState0.texture_coord_array.size, 
                                        tmuState0.texture_coord_array.type, 
                                        tmuState0.texture_coord_array.stride, 
                                        tmuState0.texture_coord_array.ptr);
        }
 
    if (arraysValid || tmuState1.texture_coord_array.changed)
        {
        tmuState1.texture_coord_array.changed = GL_FALSE;
        glClientActiveTexture(GL_TEXTURE1);
        if (tmuState1.texture_coord_array.enabled)
            {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        else
            {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        glTexCoordPointer(tmuState1.texture_coord_array.size, 
                                        tmuState1.texture_coord_array.type, 
                                        tmuState1.texture_coord_array.stride, 
                                        tmuState1.texture_coord_array.ptr);
        }

    arraysValid = GL_FALSE;
    glDrawElements(mode, count, type, indices);
    }

void qglEnableClientState(GLenum  array) 
    {
    struct nanotmuState* clientstate = NULL;
    if (clientactivetmu == GL_TEXTURE0)
        {
        clientstate = &tmuState0;
        }
    else if (clientactivetmu == GL_TEXTURE1)
        {
        clientstate = &tmuState1;
        }
    else
        {
        return;
        }
    switch (array)
        {
        case GL_VERTEX_ARRAY:
            if (clientstate->vertex_array.enabled)
                {
                return;
                }
            clientstate->vertex_array.enabled = GL_TRUE;
            clientstate->vertex_array.changed = GL_TRUE;
            break;
        case GL_COLOR_ARRAY:
            if (clientstate->color_array.enabled)
                {
                return;
                }
            clientstate->color_array.enabled = GL_TRUE;
            clientstate->color_array.changed = GL_TRUE;

            break;
        case GL_TEXTURE_COORD_ARRAY:
            if (clientstate->texture_coord_array.enabled)
                {
                return;
                }
            clientstate->texture_coord_array.enabled = GL_TRUE;
            clientstate->texture_coord_array.changed = GL_TRUE;
            break;
        default:
            break;
        }
    }
void qglDisableClientState(GLenum  array) 
    {
    struct nanotmuState* clientstate = NULL;
    if (clientactivetmu == GL_TEXTURE0)
        {
        clientstate = &tmuState0;
        }
    else if (clientactivetmu == GL_TEXTURE1)
        {
        clientstate = &tmuState1;
        }
    else
        {
        return;
        }
    switch (array)
        {
        case GL_VERTEX_ARRAY:
            if (!clientstate->vertex_array.enabled)
                {
                return;
                }
            clientstate->vertex_array.enabled = GL_FALSE;
            clientstate->vertex_array.changed = GL_TRUE;
            break;
        case GL_COLOR_ARRAY:
            if (!clientstate->color_array.enabled)
                {
                return;
                }
            clientstate->color_array.enabled = GL_FALSE;
            clientstate->color_array.changed = GL_TRUE;

            break;
        case GL_TEXTURE_COORD_ARRAY:
            if (!clientstate->texture_coord_array.enabled)
                {
                return;
                }
            clientstate->texture_coord_array.enabled = GL_FALSE;
            clientstate->texture_coord_array.changed = GL_TRUE;
            break;
        default:
            break;
        }
    }
void qglVertexPointer(	GLint size,	GLenum type,GLsizei	stride,	const GLvoid *pointer ) 
    {
    if (tmuState0.vertex_array.size == size &&
        tmuState0.vertex_array.stride == stride &&
        tmuState0.vertex_array.type == type &&
        tmuState0.vertex_array.ptr == pointer)
        {
        return;
        }
    tmuState0.vertex_array.size = size;
    tmuState0.vertex_array.stride = stride;
    tmuState0.vertex_array.type = type;
    tmuState0.vertex_array.ptr  = (GLvoid*)pointer;
    tmuState0.vertex_array.changed = GL_TRUE;
    }
void qglTexCoordPointer( GLint	size,  GLenum type,  GLsizei stride,  const	GLvoid *pointer	) 
    {
    struct nanotmuState* clientstate = NULL;
    if (clientactivetmu == GL_TEXTURE0)
        {
        clientstate = &tmuState0;
        }
    else if (clientactivetmu == GL_TEXTURE1)
        {
        clientstate = &tmuState1;
        }
    if (clientstate->texture_coord_array.size == size &&
        clientstate->texture_coord_array.stride == stride &&
        clientstate->texture_coord_array.type == type &&
        clientstate->texture_coord_array.ptr == pointer)
        {
        return;
        }
    clientstate->texture_coord_array.size = size;
    clientstate->texture_coord_array.stride = stride;
    clientstate->texture_coord_array.type = type;
    clientstate->texture_coord_array.ptr  = (GLvoid*)pointer;
    clientstate->texture_coord_array.changed = GL_TRUE;
    }
void qglColorPointer( GLint size,  GLenum type,  GLsizei stride,  const GLvoid *pointer ) 
    {
    if (tmuState0.color_array.size == size &&
        tmuState0.color_array.stride == stride &&
        tmuState0.color_array.type == type &&
        tmuState0.color_array.ptr == pointer)
        {
        return;
        }
    tmuState0.color_array.size = size;
    tmuState0.color_array.stride = stride;
    tmuState0.color_array.type = type;
    tmuState0.color_array.ptr  = (GLvoid*)pointer;
    tmuState0.color_array.changed = GL_TRUE;
    }
void qglPolygonOffset( GLfloat factor, GLfloat units ) 
    {
    FlushOnStateChange();
    glPolygonOffset(factor, units);
    }
void qglStencilMask( GLuint mask ) {}
void qglClearStencil( GLint s ) {}

#if defined(__MULTITEXTURE_SUPPORT__)

extern "C" void qglMultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t );

void qglMultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t )
    {
    if (target == GL_TEXTURE0)
        {
        qglTexCoord2f(s,t);
        }
    else
        {
        currentVertexAttrib.s_multi = s;
        currentVertexAttrib.t_multi = t;
        }
    }
#endif

/* Vladimir  */
void qglDrawArrays( GLenum mode, int first, int count)
{
    FlushOnStateChange();
    glDrawArrays(mode, first , count);
}
void qglMultMatrixf (const GLfloat *m)
{
    FlushOnStateChange();
    glMultMatrixf(m);
}

void qglPixelStorei (GLenum pname, GLint param)
{
    FlushOnStateChange();
    glPixelStorei(pname, param);
}


void qglFogf (GLenum pname, GLfloat param)
{
	FlushOnStateChange();
	glFogf(pname, param);
}

void qglFogfv (GLenum pname, const GLfloat *params)
{
	FlushOnStateChange();
	glFogfv(pname, params);
}

void qglGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
	FlushOnStateChange();
	glGetTexParameteriv(target, pname, params);
}

void qglTexParameteri (GLenum target, GLenum pname, GLint param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by openqgl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}

	FlushOnStateChange();
	glTexParameteri(target, pname, param);
}

void qglTexParameterx (GLenum target, GLenum pname, GLfixed param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by openqgl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}
	FlushOnStateChange();
	glTexParameterx(target, pname, param);
}

void qglGenTextures (GLsizei n, GLuint *textures)
{
	FlushOnStateChange();
	glGenTextures(n, textures);
}

void qglFrontFace (GLenum mode)
{
	FlushOnStateChange();
	glFrontFace(mode);
}

void qglNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
	FlushOnStateChange();
	glNormal3f(nx, ny, nz);
}

void qglMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by openqgl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}

	FlushOnStateChange();
	glMaterialf (face, pname, param);
}

void qglMaterialfv (GLenum face, GLenum pname, const GLfloat *params)
{
	FlushOnStateChange();
	glMaterialfv (face, pname, params);
}

void qglTexEnvi (GLenum target, GLenum pname, GLint param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by openqgl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}

	FlushOnStateChange();
	glTexEnvi (target, pname, param);
}

void qglLightfv (GLenum light, GLenum pname, const GLfloat *params)
{
	FlushOnStateChange();
	glLightfv (light, pname, params);
}

void qglGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
	FlushOnStateChange();
	glGetLightfv (light, pname, params);
}

void qglLightModelf (GLenum pname, GLfloat param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by openqgl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}

	FlushOnStateChange();
	glLightModelf (pname, param);
}

void qglNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer)
{
	FlushOnStateChange();
	glNormalPointer (type, stride, pointer);
}

void qglLightModelfv (GLenum pname, const GLfloat *params)
{
	FlushOnStateChange();
	glLightModelfv (pname, params);
}


// End Vladimir



