/***************************************************************************
                          glu_rip.cpp  -  description

     A few convenience routines ripped from MesaGL -- don't hurt me please
 ***************************************************************************/

#include <stdio.h>

#include "glu_rip.h"

#define CEILING( A, B )  ( (A) % (B) == 0 ? (A)/(B) : (A)/(B)+1 )

enum {
  /* Quadric draw styles */
  GLU_SMOOTH	= 100000,
  GLU_FLAT	= 100001,
  GLU_NONE	= 100002,

	/* Quadric draw styles */
  GLU_POINT	= 100010,
  GLU_LINE	= 100011,
  GLU_FILL	= 100012,
  GLU_SILHOUETTE	= 100013,

	/* Quadric orientation */
  GLU_OUTSIDE	= 100020,
  GLU_INSIDE	= 100021,

	/* Tesselator */
  GLU_BEGIN	= 100100,
  GLU_VERTEX	= 100101,
  GLU_END		= 100102,
  GLU_ERROR	= 100103,
  GLU_EDGE_FLAG	= 100104,

  /* Contour types */
  GLU_CW		= 100120,
  GLU_CCW		= 100121,
  GLU_INTERIOR	= 100122,
  GLU_EXTERIOR	= 100123,
  GLU_UNKNOWN	= 100124,

  /* Tesselation errors */
  GLU_TESS_ERROR1	= 100151,  /* missing gluEndPolygon */
  GLU_TESS_ERROR2 = 100152,  /* missing gluBeginPolygon */
  GLU_TESS_ERROR3 = 100153,  /* misoriented contour */
  GLU_TESS_ERROR4 = 100154,  /* vertex/edge intersection */
  GLU_TESS_ERROR5 = 100155,  /* misoriented or self-intersecting loops */
  GLU_TESS_ERROR6 = 100156,  /* coincident vertices */
  GLU_TESS_ERROR7 = 100157,  /* all vertices collinear */
  GLU_TESS_ERROR8 = 100158,  /* intersecting edges */
  GLU_TESS_ERROR9 = 100159,  /* not coplanar contours */

  /* NURBS */
  GLU_AUTO_LOAD_MATRIX	= 100200,
  GLU_CULLING		= 100201,
  GLU_PARAMETRIC_TOLERANCE= 100202,
  GLU_SAMPLING_TOLERANCE	= 100203,
  GLU_DISPLAY_MODE	= 100204,
  GLU_SAMPLING_METHOD	= 100205,
  GLU_U_STEP		= 100206,
  GLU_V_STEP		= 100207,

  GLU_PATH_LENGTH		= 100215,
  GLU_PARAMETRIC_ERROR	= 100216,
  GLU_DOMAIN_DISTANCE	= 100217,

  GLU_MAP1_TRIM_2		= 100210,
  GLU_MAP1_TRIM_3		= 100211,

  GLU_OUTLINE_POLYGON	= 100240,
  GLU_OUTLINE_PATCH	= 100241,

  GLU_NURBS_ERROR1  = 100251,   /* spline order un-supported */
  GLU_NURBS_ERROR2  = 100252,   /* too few knots */
  GLU_NURBS_ERROR3  = 100253,   /* valid knot range is empty */
  GLU_NURBS_ERROR4  = 100254,   /* decreasing knot sequence */
  GLU_NURBS_ERROR5  = 100255,   /* knot multiplicity > spline order */
  GLU_NURBS_ERROR6  = 100256,   /* endcurve() must follow bgncurve() */
  GLU_NURBS_ERROR7  = 100257,   /* bgncurve() must precede endcurve() */
  GLU_NURBS_ERROR8  = 100258,   /* ctrlarray or knot vector is NULL */
  GLU_NURBS_ERROR9  = 100259,   /* can't draw pwlcurves */
  GLU_NURBS_ERROR10 = 100260,   /* missing gluNurbsCurve() */
  GLU_NURBS_ERROR11 = 100261,   /* missing gluNurbsSurface() */
  GLU_NURBS_ERROR12 = 100262,   /* endtrim() must precede endsurface() */
  GLU_NURBS_ERROR13 = 100263,   /* bgnsurface() must precede endsurface() */
  GLU_NURBS_ERROR14 = 100264,   /* curve of improper type passed as trim curve */
  GLU_NURBS_ERROR15 = 100265,   /* bgnsurface() must precede bgntrim() */
  GLU_NURBS_ERROR16 = 100266,   /* endtrim() must follow bgntrim() */
  GLU_NURBS_ERROR17 = 100267,   /* bgntrim() must precede endtrim()*/
  GLU_NURBS_ERROR18 = 100268,   /* invalid or missing trim curve*/
  GLU_NURBS_ERROR19 = 100269,   /* bgntrim() must precede pwlcurve() */
  GLU_NURBS_ERROR20 = 100270,   /* pwlcurve referenced twice*/
  GLU_NURBS_ERROR21 = 100271,   /* pwlcurve and nurbscurve mixed */
  GLU_NURBS_ERROR22 = 100272,   /* improper usage of trim data type */
  GLU_NURBS_ERROR23 = 100273,   /* nurbscurve referenced twice */
  GLU_NURBS_ERROR24 = 100274,   /* nurbscurve and pwlcurve mixed */
  GLU_NURBS_ERROR25 = 100275,   /* nurbssurface referenced twice */
  GLU_NURBS_ERROR26 = 100276,   /* invalid property */
  GLU_NURBS_ERROR27 = 100277,   /* endsurface() must follow bgnsurface() */
  GLU_NURBS_ERROR28 = 100278,   /* intersecting or misoriented trim curves */
  GLU_NURBS_ERROR29 = 100279,   /* intersecting trim curves */
  GLU_NURBS_ERROR30 = 100280,   /* UNUSED */
  GLU_NURBS_ERROR31 = 100281,   /* unconnected trim curves */
  GLU_NURBS_ERROR32 = 100282,   /* unknown knot error */
  GLU_NURBS_ERROR33 = 100283,   /* negative vertex count encountered */
  GLU_NURBS_ERROR34 = 100284,   /* negative byte-stride */
  GLU_NURBS_ERROR35 = 100285,   /* unknown type descriptor */
  GLU_NURBS_ERROR36 = 100286,   /* null control point reference */
  GLU_NURBS_ERROR37 = 100287,   /* duplicate point on pwlcurve */

	/* Errors */
  GLU_INVALID_ENUM		= 100900,
  GLU_INVALID_VALUE		= 100901,
  GLU_OUT_OF_MEMORY		= 100902,
  GLU_INCOMPATIBLE_GL_VERSION	= 100903,

	/* New in GLU 1.1 */
  GLU_VERSION	= 100800,
  GLU_EXTENSIONS	= 100801
};


void gluPerspective( GLdouble fovy, GLdouble aspect,
                     GLdouble zNear, GLdouble zFar )
{
  GLdouble xmin, xmax, ymin, ymax;

  ymax = zNear * tan( fovy * M_PI / 360.0 );
  ymin = -ymax;

  xmin = ymin * aspect;
  xmax = ymax * aspect;

  glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


void gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
                GLdouble centerx, GLdouble centery, GLdouble centerz,
                GLdouble upx, GLdouble upy, GLdouble upz )
{
  GLdouble m[16];
  GLdouble x[3], y[3], z[3];
  GLdouble mag;

  /* Make rotation matrix */

  /* Z vector */
  z[0] = eyex - centerx;
  z[1] = eyey - centery;
  z[2] = eyez - centerz;
  mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
  if (mag) {  /* mpichler, 19950515 */
    z[0] /= mag;
    z[1] /= mag;
    z[2] /= mag;
  }

  /* Y vector */
  y[0] = upx;
  y[1] = upy;
  y[2] = upz;

  /* X vector = Y cross Z */
  x[0] =  y[1]*z[2] - y[2]*z[1];
  x[1] = -y[0]*z[2] + y[2]*z[0];
  x[2] =  y[0]*z[1] - y[1]*z[0];

  /* Recompute Y = Z cross X */
  y[0] =  z[1]*x[2] - z[2]*x[1];
  y[1] = -z[0]*x[2] + z[2]*x[0];
  y[2] =  z[0]*x[1] - z[1]*x[0];

  /* mpichler, 19950515 */
  /* cross product gives area of parallelogram, which is < 1.0 for
   * non-perpendicular unit-length vectors; so normalize x, y here
   */

  mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
  if (mag) {
    x[0] /= mag;
    x[1] /= mag;
    x[2] /= mag;
  }

  mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
  if (mag) {
    y[0] /= mag;
    y[1] /= mag;
    y[2] /= mag;
  }

#define M(row,col)  m[col*4+row]
  M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
  M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
  M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
  M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
  glMultMatrixd( m );

  /* Translate Eye to Origin */
  glTranslatef( (float) -eyex, (float) -eyey, (float) -eyez );

}





