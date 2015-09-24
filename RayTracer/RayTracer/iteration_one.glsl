#version 430
layout(binding=0, rgba8) uniform image2D outputTexture;

uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;

struct box {
	vec3 min;
	vec3 max;
};

struct sphere {
	vec3 pos;
	float r;
};

struct triangle {
	vec3 p0;
	vec3 p1;
	vec3 p2;
};

#define MAX_SCENE_BOUNDS 100.0
#define NUM_BOXES 2
#define NUM_SPHERES 2
#define NUM_TRIANGLES 2
//#define NUM_LIGHTS 1

const box boxes[] = {
	/* The ground */
	{vec3(-5.0, -0.1, -5.0), vec3(5.0, 0.0, 5.0)},
	/* Box in the middle */
	{vec3(-0.5, 0.0, -0.5), vec3(0.5, 1.0, 0.5)}
};

const sphere spheres[] = {
	/* The ground */
	{vec3(5.0, -0.1, -5.0), 1.0},
	/* Box in the middle */
	{vec3(-0.5, 5.0, -0.5), 1.0}
};

const triangle triangles[] = {
	/* The ground */
	{vec3(5.0, -5.1, -6.0), vec3(5.0, -5.1, -4.0), vec3(-5.0, -5.1, -5.0)},
	{vec3(5.0, -5.1, -6.0), vec3(5.0, -2.1, -4.0), vec3(-5.0, -5.1, -5.0)}
};

//const vec3 lights[] {
//	vec3(1,1,1)
//};

struct hitinfo {
	vec2 lambda;
	int bi;
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
//// 			Box collision
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

vec2 intersectBox(vec3 origin, vec3 dir, const box b) {
	vec3 tMin = (b.min - origin) / dir;
	vec3 tMax = (b.max - origin) / dir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);
	return vec2(tNear, tFar);
}

bool intersectBoxes(vec3 origin, vec3 dir, out hitinfo info) {
	float smallest = MAX_SCENE_BOUNDS;
	bool found = false;
	for (int i = 0; i < NUM_BOXES; i++) {
		vec2 lambda = intersectBox(origin, dir, boxes[i]);
		if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest) {
			info.lambda = lambda;
			info.bi = i;
			smallest = lambda.x;
			found = true;
		}
	}
	return found;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
//// 			Sphere collision
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


vec2 intersectSphere(vec3 origin, vec3 dir, const sphere s) {
//	float b = dot(dir, origin - s.pos);
//	float c = dot(origin - s.pos, origin - s.pos) - powf(s.r, 2.0f);
//
//	return powf(b, 2.0f) - c;
	
	//Squared distance between ray origin and sphere center
    float squaredDist = dot(origin - s.pos, origin - s.pos);

    //If the distance is less than the squared radius of the sphere...
    if(squaredDist <= s.r)
    {
        //Point is in sphere, consider as no intersection existing
        //std::cout << "Point inside sphere..." << std::endl;
        return vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
    }

    //Will hold solution to quadratic equation
    float t0, t1;

    //Calculating the coefficients of the quadratic equation
    float a = dot(dir,dir); // a = d*d
    float b = 2.0 * dot(dir, origin - s.pos); // b = 2d(o-C)
    float c = dot(origin - s.pos, origin - s.pos) - (s.r*s.r); // c = (o-C)^2-R^2

    //Calculate discriminant
    float disc = (b*b)-(4.0*a*c);

    if(disc < 0) //If discriminant is negative no intersection happens
    {
        //std::cout << "No intersection with sphere..." << std::endl;
        return vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
    }
    else //If discriminant is positive one or two intersections (two solutions) exists
    {
        float sqrt_disc = sqrt(disc);
        t0 = (-b - sqrt_disc) / (2.0f * a);
        t1 = (-b + sqrt_disc) / (2.0f * a);
		return vec2(min(t0, t1), max(t0, t1));
    }
}

bool intersectSpheres(vec3 origin, vec3 dir, out hitinfo info) {
	float smallest = MAX_SCENE_BOUNDS;
	bool found = false;
	for (int i = 0; i < NUM_SPHERES; i++) {
		vec2 lambda = intersectSphere(origin, dir, spheres[i]);
		if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest) {
			info.lambda = lambda;
			info.bi = i;
			smallest = lambda.x;
			found = true;
		}
	}
	return found;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
//// 			Triangle collision
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


vec2 intersectTriangle(vec3 origin, vec3 dir, const triangle tri) {
	vec3 p0, p1, p2;

	p0 = tri.p0;
	p1 = tri.p1;
	p2 = tri.p2;

	vec3 e1 = p1 - p0;
	vec3 e2 = p2 - p0;

	vec3 q = cross(dir, e2);
	float a = dot(e1, q);

	if(a > -pow(10, -20) && a < pow(10, -20))
	{
		return vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
	}

	float f = 1.0f/a;
	vec3 s = origin - p0;
	float u = dot(dot(s,q),f);

	if(u < 0.0f)
	{
		return vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
	}

	vec3 r = cross(s,e1);
	float v = dot(dot(dir,r),f);

	if(v < 0.0f || u + v > 1.0f)
	{
		return vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
	}

	float t = dot(dot(e2,r),f);
	return vec2(t, MAX_SCENE_BOUNDS);
}

bool intersectTriangles(vec3 origin, vec3 dir, out hitinfo info) {
	float smallest = MAX_SCENE_BOUNDS;
	bool found = false;
	for (int i = 0; i < NUM_TRIANGLES; i++) {
		vec2 lambda = intersectTriangle(origin, dir, triangles[i]);
		if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest) {
			info.lambda = lambda;
			info.bi = i;
			smallest = lambda.x;
			found = true;
		}
	}
	return found;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
//// 			Trace
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

vec4 trace(vec3 origin, vec3 dir) {
	vec4 ret = vec4(1.0, 0.5, 0.0, 1.0);
	hitinfo i;
	i.lambda = vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
	i.bi = -1;
	hitinfo i2;
	i2.lambda = vec2(MAX_SCENE_BOUNDS, MAX_SCENE_BOUNDS);
	i2.bi = -1;
	if(intersectSpheres(origin, dir, i)) {
		ret = vec4(i.bi / 5.0 + 0.8, 1.0, 0.0, 1.0);
		i2 = i;
		//return vec4(gray.rg, 0.4, 1.0);
	} 
	if (intersectBoxes(origin, dir, i)) {
		if(i2.lambda.x > i.lambda.x) {
			ret = vec4(i.bi / 10.0 + 0.8);
			//return vec4(gray.rgb, 1.0);
			i2 = i;
		}
	}
	if (intersectTriangles(origin, dir, i)) {
		if(i2.lambda.x > i.lambda.x) {
			ret = vec4(i.bi / 5.0 + 0.8, 0.0, 1.0, 1.0);
			//return vec4(gray.rgb, 1.0);
			i2 = i;
		}
	}
	
	
	
	return ret;
}

layout (local_size_x = 16, local_size_y = 16) in;
void main(void) {
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(outputTexture);
	if (pix.x >= size.x || pix.y >= size.y) {
		return;
	}
	vec2 pos = vec2(pix) / vec2(size.x - 1, size.y - 1);
	vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
	vec4 color = trace(eye, dir);
	imageStore(outputTexture, pix, color);
}

