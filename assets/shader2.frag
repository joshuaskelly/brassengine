#version 100
precision mediump float;
uniform sampler2D screen_texture;
varying mediump vec2 uv;
void main() {
    gl_FragColor = texture2D(screen_texture, uv);
}
