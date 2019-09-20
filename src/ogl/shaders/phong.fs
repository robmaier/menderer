/**
* This file is part of Menderer.
*
* Copyright 2019 Robert Maier, Technical University of Munich.
* For more information see <https://github.com/robmaier/menderer>.
*
* Menderer is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Menderer is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Menderer. If not, see <http://www.gnu.org/licenses/>.
*/

#version 120


varying vec3 normal;
varying vec3 vpos;

void main()
{
    // vectors for shading computation
    vec3 n = normalize(normal);
    vec3 light_dir = normalize(gl_LightSource[0].position.xyz - vpos);
    vec3 E = normalize(-vpos);
    vec3 R = normalize(-reflect(light_dir, n));

    // ambient term
    vec4 ambient = gl_FrontLightProduct[0].ambient;
    // diffuse term
    vec4 diffuse = gl_FrontLightProduct[0].diffuse * max(dot(n, light_dir), 0.0);
    diffuse = clamp(diffuse, 0.0, 1.0);
    // specular term
    vec4 specular = gl_FrontLightProduct[0].specular * pow(max(dot(R, E), 0.0), 0.3 * gl_FrontMaterial.shininess);
    specular = clamp(specular, 0.0, 1.0); 

    // output color
    gl_FragColor = gl_FrontLightModelProduct.sceneColor + ambient + diffuse + specular;
}
