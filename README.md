# AC Worldgen
![](img/4.png)
This is a repository for a worldgen system for AnotherCraft. The system is completely standalone and can be used for world generation in any voxel-based game.

## System premise & features
* The whole thing is based around discretized fields, every variable/value is a field that can have a different value for each voxel in the world.
* By doing various operations on these fields (incl. stuff like perlin noises, voronoi stuff, ...), you can achieve some pretty cool results.
* There's also a little more linearistic **procedural structure generator** system.
  * The system can also **import `.vox` files**.

## System structure
* WorldGenAPI, which is a parallel-enabled worldgen system backend (can also be used standalone, the generation pipeline can be constructed using API calls)
* The WOGLAC (WOrldGen Language for AnotherCraft) programming language compiler that compiles the WOGLAC code into WorldGenAPI calls.
  * Language documentation can be found in the [ac-docs repository](https://github.com/AnotherCraft/ac-docs/tree/master/woglac).
  * Files in `woglac/parser` are autogenerated by Antlr4 from grammar `grammar/Woglac.g4` (you can use the `grammar/gen_parser.sh` script).
  
## Documentation & resources
* [Language reference](https://github.com/AnotherCraft/ac-docs/blob/master/woglac/language_spec.md)
* [Function list](https://github.com/AnotherCraft/ac-docs/blob/master/woglac/function_list.md)
* [Worldgen basics (youtube video)](https://www.youtube.com/watch?v=yqHEID7LIU4)
* [VS Code extension for WOGLAC](https://github.com/AnotherCraft/ac-woglac-vscode)

### WOGLAC example code
See the [examples](example) folder.
```WOGLAC
// That 0.001 is there because weighted biome variable interpolation introduces some imperfections
Float z = worldPos()::z() + 0.5;
Float terrainZ = 195;

// That #342 is a function-local seed.
// The seeds are entered manually like this so that even when moving the function around, it still produces the same results. 
export Block resultBlock = z < terrainZ ? (rand2D(#342) < 0.5 ? block.core.dirt : block.core.grass) : block.air;
```

## Third-party resources
* Qt6
* Antlr4
* Tracy

## TODO
* Get rid of the Qt dependency (it's not really that necessary)