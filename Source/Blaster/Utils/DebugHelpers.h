#pragma once

// Macro for drawing debug shapes in development or test configuration and ignore them in Shipping builds. 
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#define DRAW_DEBUG_SPHERE(World,Location,Radius,Segments,Color,Persistent) \
DrawDebugSphere(World, Location, Radius, Segments, Color, Persistent)
#else
#define DRAW_DEBUG_SPHERE(World,Location,Radius,Segments,Color,Persistent)
#endif