#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_col;
layout(location = 2) in vec2 a_tex_coord;

out vec3 our_color;
out vec2 tex_coord;

uniform vec3 hue;

vec3 HSV2RGB(vec3 _HSV);

void main() {
  gl_Position = vec4(a_pos, 1.0);
  our_color = HSV2RGB(vec3(a_col.x + hue.x, a_col.yz));
  tex_coord = vec2(a_tex_coord.x, a_tex_coord.y);
}

vec3 HSV2RGB(vec3 _HSV) {
  _HSV.x = mod(100.0 + _HSV.x, 1.0); // Ensure [0,1[

  float HueSlice = 6.0 * _HSV.x; // In [0,6[
  float HueSliceInteger = floor(HueSlice);
  float HueSliceInterpolant =
      HueSlice - HueSliceInteger; // In [0,1[ for each hue slice

  vec3 TempRGB = vec3(_HSV.z * (1.0 - _HSV.y),
                      _HSV.z * (1.0 - _HSV.y * HueSliceInterpolant),
                      _HSV.z * (1.0 - _HSV.y * (1.0 - HueSliceInterpolant)));

  // The idea here to avoid conditions is to notice that the conversion code can
  // be rewritten:
  //    if      ( var_i == 0 ) { R = V         ; G = TempRGB.z ; B = TempRGB.x }
  //    else if ( var_i == 2 ) { R = TempRGB.x ; G = V         ; B = TempRGB.z }
  //    else if ( var_i == 4 ) { R = TempRGB.z ; G = TempRGB.x ; B = V     }
  //
  //    else if ( var_i == 1 ) { R = TempRGB.y ; G = V         ; B = TempRGB.x }
  //    else if ( var_i == 3 ) { R = TempRGB.x ; G = TempRGB.y ; B = V     }
  //    else if ( var_i == 5 ) { R = V         ; G = TempRGB.x ; B = TempRGB.y }
  //
  // This shows several things:
  //  . A separation between even and odd slices
  //  . If slices (0,2,4) and (1,3,5) can be rewritten as basically being slices
  //  (0,1,2) then
  //      the operation simply amounts to performing a "rotate right" on the RGB
  //      components
  //  . The base value to rotate is either (V, B, R) for even slices or (G, V,
  //  R) for odd slices
  //
  float IsOddSlice =
      mod(HueSliceInteger,
          2.0); // 0 if even (slices 0, 2, 4), 1 if odd (slices 1, 3, 5)
  float ThreeSliceSelector =
      0.5 *
      (HueSliceInteger -
       IsOddSlice); // (0, 1, 2) corresponding to slices (0, 2, 4) and (1, 3, 5)

  vec3 ScrollingRGBForEvenSlices = vec3(
      _HSV.z, TempRGB.zx); // (V, Temp Blue, Temp Red) for even slices (0, 2, 4)
  vec3 ScrollingRGBForOddSlices =
      vec3(TempRGB.y, _HSV.z,
           TempRGB.x); // (Temp Green, V, Temp Red) for odd slices (1, 3, 5)
  vec3 ScrollingRGB =
      mix(ScrollingRGBForEvenSlices, ScrollingRGBForOddSlices, IsOddSlice);

  float IsNotFirstSlice =
      clamp(ThreeSliceSelector, 0.0,
            1.0); // 1 if NOT the first slice (true for slices 1 and 2)
  float IsNotSecondSlice =
      clamp(ThreeSliceSelector - 1.0, 0.0,
            1.0); // 1 if NOT the first or second slice (true only for slice 2)

  return mix(ScrollingRGB.xyz,
             mix(ScrollingRGB.zxy, ScrollingRGB.yzx, IsNotSecondSlice),
             IsNotFirstSlice); // Make the RGB rotate right depending on final
                               // slice index
}
