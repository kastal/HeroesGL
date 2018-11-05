/*
	Cubic fragment shader
	based on libretro Cubic shader
	https://github.com/libretro/glsl-shaders/tree/master/cubic/shaders

	MIT License

	Copyright (c) 2018 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#version 130
precision mediump float;

uniform sampler2D tex01;

in vec2 fTexCoord;

out vec4 fragColor;

void weights(out vec4 x, out vec4 y, vec2 t)
{
   vec2 t2 = t * t;
   vec2 t3 = t2 * t;

   vec4 xs = vec4(1.0, t.x, t2.x, t3.x);
   vec4 ys = vec4(1.0, t.y, t2.y, t3.y);

   const vec4 p0 = vec4(+0.0, -0.5, +1.0, -0.5);
   const vec4 p1 = vec4(+1.0,  0.0, -2.5, +1.5);
   const vec4 p2 = vec4(+0.0, +0.5, +2.0, -1.5);
   const vec4 p3 = vec4(+0.0,  0.0, -0.5, +0.5);

   x = vec4(dot(xs, p0), dot(xs, p1), dot(xs, p2), dot(xs, p3));
   y = vec4(dot(ys, p0), dot(ys, p1), dot(ys, p2), dot(ys, p3));
}

void main()
{
	vec2 inSize = textureSize(tex01, 0);
	vec2 uv = fTexCoord * inSize - 0.5;
	vec2 texel = floor(uv);
	vec2 tex = (texel + 0.5) / inSize;
	vec2 phase = uv - texel;

	#define TEX(x, y) textureLodOffset(tex01, tex, 0.0, ivec2(x, y)).rgb

	vec4 x;
	vec4 y;
	weights(x, y, phase);

	vec4 row = x * y.x;
	vec3 color = TEX(-1, -1) * row.x;
	color += TEX(+0, -1) * row.y;
	color += TEX(+1, -1) * row.z;
	color += TEX(+2, -1) * row.w;

	row = x * y.y;
	color += TEX(-1, +0) * row.x;
	color += TEX(+0, +0) * row.y;
	color += TEX(+1, +0) * row.z;
	color += TEX(+2, +0) * row.w;

	row = x * y.z;
	color += TEX(-1, +1) * row.x;
	color += TEX(+0, +1) * row.y;
	color += TEX(+1, +1) * row.z;
	color += TEX(+2, +1) * row.w;

	row = x * y.w;
	color += TEX(-1, +2) * row.x;
	color += TEX(+0, +2) * row.y;
	color += TEX(+1, +2) * row.z;
	color += TEX(+2, +2) * row.w;

	fragColor = vec4(color, 1.0);
} 