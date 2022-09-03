# Life cell sim
This is a basic thing in C++ that simulates parametered gravity. It is inspired by this video: https://www.youtube.com/watch?v=0Kx4Y9TVMGg.

It reads [simulation.ini](simulation.ini) from the current dir, and uses that to build a simulation.

It has 2 fields it scans for at all times, `[Simulation]` and `[Colors]`.  
`[Simulation]` is optional, `[Colors]` is not.  

Under `[Colors]` you are expected to put in a cell type name(anything), followed by
`= [amt]`, with `[amt]` being the amount of cells.  
```ini
[Colors]
green=200
red=150
```

Then we've got `[Simulation]`, this is an optional segment for optional tweaking.
In it, you've got the following
```ini
[Simulation]
x=512 # The with of the simulation playground(1280 by default)
y=1024 # The height of the simulation playground(720 by default)
exp_grav=false # Alternative gravity method(true by default)
target_fps=60 # The window(and simulation) target FPS(60 by default)
```

Then there are the particle fields, these are where the magic happens.
They are structured as such:
```ini
[particle_name] # E.g green or red
size=1 # Optional, 2 by default
color=11 # The color of the particle, based of off the PICO-8 color palette(0-15)
any_particle_name=-5 # The repulsion/traction to any particle type, including self, positive is repulse, negative is contract
```

Example `simulation.ini`
```ini
[Simulation]
exp_grav=false
x=500
y=500
target_fps=60

[Colors]
yellow=300
red=300
green=400

[yellow]
color=10
yellow=-10
red=5
green=-5

[red]
size=1
color=8
red=-1
yellow=-10
green=5

[green]
size=1
color=11
green=-1
red=-1
yellow=-5
```