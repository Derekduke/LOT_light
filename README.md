# LOT_light
This is an actual engineering project designed for a Chinese company, which solves the problem of stable network communication of outdoor Internet street lights and can work with low power consumption. So most of the comments in the code are in Chinese. The Cortex-0 architecture of the master station uses STM32, while the slave station uses the low-power version of STM8. The code is opened using MDK Version 5 or IAR. In order to reduce power consumption and memory overhead, this version of the code does not use an operating system, but has designed a simple real-time system framework.

packge_struct_draft.c and project_design_draft.c are not part of the engineering code, but are the idea of the communication protocol design.
