# raytrace
Raytracing in C

4000x4000 render ([scenes/example](https://github.com/longwatermelon/raytrace/blob/master/scenes/example))
![out](https://user-images.githubusercontent.com/73869536/161472071-2e7adc69-382c-4e79-a805-0c0f3f9fb463.png)

# Runtime dependencies
* [ffmpeg](https://ffmpeg.org/)

# Building
```
git clone https://github.com/longwatermelon/raytrace
cd raytrace
make
```

`raytrace [config file]` to raytrace an image or a video. At the top of the file, write either `image` to raytrace an image or `video` to raytrace a video.

<details>
  <summary>raytrace scenes/example</summary>
  
  ![0](https://user-images.githubusercontent.com/73869536/161469414-7f5bf90e-14ff-4511-8278-a62637603663.png)
  </details>

<details>
  <summary>raytrace videos/example</summary>

  https://user-images.githubusercontent.com/73869536/161469337-59b1b403-961d-43e5-a5bd-f1fd11295f46.mp4
  </details>
