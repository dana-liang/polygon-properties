/* CS211 Summer 2020 Project 4
 *
 * dliang26-proj4.c
 * By Dana Liang, 5/31/2020
 * 
 * This program maintains a collection of Polygons and their properties:
 * their vertices, luminosity, and their bounds.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

int debugMode = FALSE; // Global variable: print debugging information if TRUE

/* Point3d data type */
typedef struct {
     int ID; 		    // unique number for each vertex; will specify which vertices make up a polygon
     double X, Y, Z; 	// coordinates for the 3D point
     double luminosity; // brightness of vertex, ranges from 0.0 - 1.0
} Point3d;

/* PointCloud data type */
typedef struct {
     Point3d * points; 	// array to hold all points in the PointCloud (grow as needed)
     int allocated; 	// keep track of how many Point3d's have been allocated in the points array
     int nPoints; 		// keep track of how many points are in the points array
     double avgLum; 	// average luminosity of the points in the PointCloud (0 if 0 points inside)
     double xMin, xMax, yMin, yMax, zMin, zMax; // minimum and maximum values of all points (all 0 if 0 points inside)
} PointCloud;

/* PointNode data type */
typedef struct pointNode {
    Point3d* point;         // pointer to a Point3d somewhere in a PointCloud
    struct pointNode* next;	// pointer to the next PointNode in the list
} PointNode;

/* Polygon data type */
typedef struct {
    int ID;             // unique identifier for the Polygon, starting at 0
    PointNode* points;	// head of the linked list of PointNodes
    int nPoints;	    // number of points in the Polygon currently
    double avgLum;      // average luminosity of all vertices in the polygon (0 if 0 points inside)
    double xMin, xMax, yMin, yMax, zMin, zMax; // min and max values for all vertices (0 if 0 points inside)
} Polygon;

/************************* 
 * FUNCTION DECLARATIONS *
 *************************/
int addPointToCloud( PointCloud* cloud, Point3d point );

Point3d* findPointAddress( PointCloud cloud, int ID);

int addVertexToPolygon( Polygon* polygon, int vertexID, PointCloud* allPoints );

int polygonPointIntersection( Polygon poly, Point3d point );

int size( Polygon polygon );

int isEmpty( Polygon polygon );

int removeVertexFromPolygon( Polygon *polygon );

/****************** 
 *      MAIN      *
 ******************/

int main ( int argc, char const *argv[] )
{
    /* Print information for debugging if "-d" is given as a command line argument */
    for ( int i = 0; i < argc; i++ )
    {
        if ( strcmp ("-d", argv[i]) == 0 )
		debugMode = TRUE;
	else
		debugMode = FALSE;
    }

    /* Allocate space for a PointCloud, and an array of Point3d inside PointCloud */
    PointCloud *allPoints;
    allPoints = (PointCloud*) malloc ( sizeof(PointCloud) );

    allPoints->allocated = 100;
    allPoints->points = (Point3d*) malloc ( allPoints->allocated * sizeof(Point3d) );

    Point3d point;
    
    printf("Enter a list of X, Y, and Z values followed by a luminosity value. Terminate your list with a negative luminosity value:\n");
    scanf("%lf", & (point.X) );
    scanf("%lf", & (point.Y) );
    scanf("%lf", & (point.Z) );
    scanf("%lf", & (point.luminosity) );
 
    /* Read data for a series of points from standard input until a negative luminosity is encountered */
    int i = 0; // Keep track of array indices and size
    while ( point.luminosity >= 0 )
    {
	point.ID = i;
	addPointToCloud( allPoints, point );
	i++;

	/* Double the array space if more points are entered than initially allocated */
	if ( (i+1) == allPoints->allocated )
	{
	    Point3d* temp = allPoints->points;
	    allPoints->points = (Point3d*) malloc ( i * 2 * sizeof(Point3d) );
	    for ( int j = 0; j < i; j++ )
		    allPoints->points[j] = temp[j];
	    free(temp);
	    allPoints->allocated = allPoints->allocated * 2;
	}
	
	/* get next values */
	scanf("%lf", & (point.X) );
	scanf("%lf", & (point.Y) );
	scanf("%lf", & (point.Z) );
	scanf("%lf", & (point.luminosity) );
    }

    allPoints->nPoints = i;

    /* Debug: check if all values are read into the allPoints->points array correctly */
    if ( debugMode == TRUE )
    {
        for ( int k = 0; k < i; k++ )
		printf("Point %d: (%.2f, %.2f, %.2f)		Lum: %.2f\n", allPoints->points[k].ID, allPoints->points[k].X, 
				allPoints->points[k].Y, allPoints->points[k].Z, allPoints->points[k].luminosity);
	printf("Total # of Points: %d\n", allPoints->nPoints);
    }

    /* Determine the upper and lower bounds of the bounding box and the average luminosity of the point cloud.
     * Store these in the PointCloud structure.*/
    
    allPoints->xMin = allPoints->points[0].X;
    allPoints->xMax = allPoints->points[0].X;
    allPoints->yMin = allPoints->points[0].Y;
    allPoints->yMax = allPoints->points[0].Y;
    allPoints->zMin = allPoints->points[0].Z;
    allPoints->zMax = allPoints->points[0].Z;
    allPoints->avgLum = 0;
    
    for ( i = 0; i < allPoints->nPoints; i++ )
    {
        if ( allPoints->points[i].X > allPoints->xMax )
            allPoints->xMax = allPoints->points[i].X;

        if ( allPoints->points[i].X < allPoints->xMin )
	        allPoints->xMin = allPoints->points[i].X;

        if ( allPoints->points[i].Y > allPoints->yMax )
            allPoints->yMax = allPoints->points[i].Y;

        if ( allPoints->points[i].Y < allPoints->yMin )
	        allPoints->yMin = allPoints->points[i].Y;

        if ( allPoints->points[i].Z > allPoints->zMax )
            allPoints->zMax = allPoints->points[i].Z;
            
        if ( allPoints->points[i].Z < allPoints->zMin )
	        allPoints->zMin = allPoints->points[i].Z;

	allPoints->avgLum = allPoints->avgLum + allPoints->points[i].luminosity;
    }

    allPoints->avgLum = allPoints->avgLum / (double)allPoints->nPoints; 

    /* Report the bounds of the Point Cloud, the average luminosity, the number of points in the Point Cloud */
    printf("\nBounding Box:\n");
    printf("X:	Lower: %.2f	Upper: %.2f\n", allPoints->xMin, allPoints->xMax);
    printf("Y:	Lower: %.2f	Upper: %.2f\n", allPoints->yMin, allPoints->yMax);
    printf("Z:	Lower: %.2f	Upper: %.2f\n", allPoints->zMin, allPoints->zMax);
    printf("Average Luminosity (nearest hundredth): %.2f\n", allPoints->avgLum);

    /* allocate space for growing array of Polygons */
    int allocated = 100; // 100 for now
    Polygon* polygons = ( Polygon* ) malloc ( allocated * sizeof(Polygon) );

    /* Build Polygons from the vertices in allPoints */
    /* Read data until a negative integer is read */
    printf("\nEnter a list of integer vertex numbers from the PointCloud to add to Polygons. Terminate your list with a negative integer.\n");

    int val;
    scanf( "%d", &val );
    i = 0; // keep track of the number of polygons in the array
    polygons[i].nPoints = 0;

    while ( val >= 0 )
    {   
        polygons[i].ID = i;
        addVertexToPolygon ( &polygons[i], val, allPoints );
        polygons[i].nPoints++;

        /* Double the array space if more points are entered than initially allocated */
	    if ( (i+1) == allocated ) {
	    Polygon* temp = polygons;
	    polygons = (Polygon*) malloc ( i * 2 * sizeof(Polygon) );
	    for ( int j = 0; j < i; j++ )
		    polygons[j] = temp[j];
	    free(temp);
	    allocated = allocated * 2;
	    }

	    scanf( "%d", &val );
        
        if ( val < 0 ) { // if the value is negative, go to the next polygon
            i++;
            polygons[i].nPoints = 0;
            scanf( "%d", &val );
            if ( val < 0 ) // if the value is negative yet again, stop the loop
                break;
        }
            continue;
    }

    /* debugging: check how many points are in the polygons */
    if ( debugMode == TRUE ) 
    {
        for ( int j = 0; j < i; j++ )
        {
            if ( isEmpty ( polygons[j] ) == 1 )
                printf("Polygon %d is empty.\n", j);
            printf("Polygon %d: # of Points: %d\n", polygons[j].ID, size( polygons[j] ) );
        }
	printf("\n");
    } 

    /* Report each Polygon's average luminosity and bounding box limits.*/
    PointNode* current = NULL;
    for ( int j = 0; j < i; j++ )
    {
        if ( isEmpty ( polygons[j] ) == 1 )
	        printf("Polygon %d is empty.\n", j);

        else
        {
            printf("\n# of vertices in the polygon: %d\n", size ( polygons[j] ) );

            /* Find the bounding box and avg luminosity of the polygon */
            polygons[j].xMin = polygons[j].points->point->X;
            polygons[j].xMax = polygons[j].points->point->X;
            polygons[j].yMax = polygons[j].points->point->Y;
            polygons[j].yMin = polygons[j].points->point->Y;
            polygons[j].zMin = polygons[j].points->point->Z;
            polygons[j].zMax = polygons[j].points->point->Z;
            polygons[j].avgLum = 0;

            for (current = polygons[j].points; current != NULL; current = current->next )
            {
                if ( current->point->X > polygons[j].xMax )
                    polygons[j].xMax = current->point->X;

                if ( current->point->X < polygons[j].xMin )
                    polygons[j].xMin = current->point->X;

                if ( current->point->Y > polygons[j].yMax )
                    polygons[j].yMax = current->point->Y;

                if ( current->point->Y < polygons[j].yMin )
                    polygons[j].yMin = current->point->Y;

                if ( current->point->Z > polygons[j].zMax )
                    polygons[j].zMax = current->point->Z;

                if ( current->point->Z < polygons[j].zMin )
                    polygons[j].zMin = current->point->Z;

              	polygons[j].avgLum = polygons[j].avgLum + current->point->luminosity;
            }

            polygons[j].avgLum = polygons[j].avgLum / (double)polygons[j].nPoints; 
            
            /* Report the bounds */
            printf("Polygon %d Bounding Box:\n", j);
            printf("X:  Min: %.2f   Max: %.2f   \n", polygons[j].xMin, polygons[j].xMax);
            printf("Y:  Min: %.2f   Max: %.2f   \n", polygons[j].yMin, polygons[j].yMax);
            printf("Z:  Min: %.2f   Max: %.2f   \n", polygons[j].zMin, polygons[j].zMax);
            printf("\n");
        }
    }
    
    /* Report Average Luminosities */
    printf("Average Luminosities:\n");
    printf("Polygon     Avg Luminosity\n");
    for ( int j = 0; j < i; j++ )
    {
        printf("%d                  %.2f\n", j, polygons[j].avgLum);
    }
    /* Read data for a second set of vertices, and for each vertex ID search the Polygon's list of vertices, 
     * if a matching vertex ID is found, report the address of the PointNode where the ID was found. */
    /* Read data until a negative integer is read */
    
        printf("\nEnter another list of XYZ points and terminate with a negative luminosity" 
		    " to check if they intersect with any of the polygons:\n");
        scanf("%lf", & (point.X) );
        scanf("%lf", & (point.Y) );
        scanf("%lf", & (point.Z) );
        scanf("%lf", & (point.luminosity) );
        
        while ( point.luminosity >= 0 )
        {   
            int intersection = FALSE; // track if there has already been an intersection
            for ( int j = 0; j < i; j++ ) // iterate through all the polygons and check if they intersect
            {
                if ( polygonPointIntersection( polygons[j], point ) == 1 )
                {
                    printf("\n(%.2f, %.2f, %.2f) intersects with Polygon %d.", point.X, point.Y,
                        point.Z, j);
                    intersection = TRUE;
                }
            }

            if ( intersection == FALSE ) // if there has been no intersection
            {
                printf("\n(%.2f, %.2f, %.2f) intersects with no Polygons.", point.X, point.Y,
                        point.Z);
            }
            
            scanf("%lf", & (point.X) );
            scanf("%lf", & (point.Y) );
            scanf("%lf", & (point.Z) );
            scanf("%lf", & (point.luminosity) );
        }

    /* Free all dynamically allocated memory before the program ends.*/
    for ( int j = 0; j < i; j++)
    {
        while ( removeVertexFromPolygon( &polygons[j] ) != -1 )  
	        removeVertexFromPolygon( &polygons[j] );
    }
    free(allPoints->points);
    free(allPoints);
    free(polygons);

    return 0;
} //END main()

/************************ 
 * FUNCTION DEFINITIONS *
 ************************/

/* Returns 0 if the point is successfully added to the cloud or -1 otherwise. */
int addPointToCloud( PointCloud* cloud, Point3d point )
{
    if ( point.ID < cloud->allocated )
    {
        cloud->points[point.ID] = point;
	return 0;
    }
    
    else
        return -1;
} 

/*************************************************/

/* Returns 0 ( FALSE ) if the Polygon has no points.
 * Otherwise check if the point's coordinates are
 * within or equal to the bounds of the cloud.
 * Return 1 ( TRUE ) if there is an intersection or 0 ( FALSE ) otherwise */
int polygonPointIntersection( Polygon poly, Point3d point )
{
    if ( point.X <= poly.xMax && point.X >= poly.xMin &&
         point.Y <= poly.yMax && point.Y >= poly.yMin &&
         point.Z <= poly.zMax && point.Z >= poly.zMin )
	    return 1;
    else
	    return 0;
} 

/*************************************************/

/* Return 0 if vertexID is successfully added to the polygon, -1 otherwise */
int addVertexToPolygon( Polygon* polygon, int vertexID, PointCloud* allPoints )
{    
    if ( polygon == NULL )
	    return -1;
    
    PointNode* temp = (PointNode*) malloc ( sizeof(PointNode) );
    temp->point = findPointAddress ( *allPoints, vertexID );
    temp->next = polygon->points;
    polygon->points = temp;

    return 0;
} 

/*************************************************/
/* Return address of the Point3d having an ID matching that passed in, or
 * NULL if the matching ID could not be found */
Point3d* findPointAddress ( PointCloud cloud, int ID ) 
{
    Point3d* temp = NULL;
    temp = &cloud.points[ID];
    return temp;
}

/*************************************************/

/* Return number of vertices in the polygon */
int size( Polygon polygon )
{
    return polygon.nPoints;
}

/*************************************************/

/* Return 1 if there are no vertices in the polygon, 0 otherwise */
int isEmpty( Polygon polygon )
{
    if ( polygon.nPoints == 0 )
	    return 1;
    else
	    return 0;
}

/*************************************************/

/* Remove 1 vertex from the polygon (from the head of the linked list)
 * and free the memory associated with that PointNode.
 * Return the ID of the removed vertex if successfully removed, -1 otherwise */
int removeVertexFromPolygon( Polygon *polygon )
{
    PointNode* temp = NULL;
    int ID;

    if ( polygon->points == NULL )
	    return -1;

    temp = polygon->points;
    ID = temp->point->ID;
    polygon->points = polygon->points->next;
    temp->next = NULL;
    free(temp);
    
    return ID;
}
