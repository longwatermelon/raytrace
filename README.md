# raytrace
Raytracing in C

4000x3000 render ([scenes/example](https://github.com/longwatermelon/raytrace/blob/master/scenes/example))
![out](https://user-images.githubusercontent.com/73869536/161411357-33d7449a-2132-4076-9327-7da3f1c8ba93.png)

# Building
```
git clone https://github.com/longwatermelon/raytrace
cd raytrace
make
```

`raytrace [config file]` to raytrace an image or a video. At the top of the file, write either `image` to raytrace an image or `video` to raytrace a video.

`raytrace scenes/example`
![0](https://user-images.githubusercontent.com/73869536/161447563-5e709170-2e8b-455d-ba01-c111c4a63a0a.png)

`raytrace videos/example`

![out](https://user-images.githubusercontent.com/73869536/161447504-6ed30bd3-0403-4713-b942-4f7677755b63.gif)
