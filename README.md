# Beat My Bot
A very simple video game featuring two teams of AI controlled bots. 

The purpose of this project was to implement our own AI for the red team, which included a state machine and A* pathfinding systems. The blue team was handled by a function given to us by the lecturer. 

This project was part of a third-year module in a Computer Games Programming course at the University of Northumbria in Newcastle upon Tyne, 2018.

## Building the Project
### A quick install of DXSDK June 2010
This project is built using a rudimentary game engine referred to as ‘The Shell’; created by lecturer Chris Rook, as an educational tool for learning C++. It relies on having the DirectX SDK June 2010 installed to your computer, found at this URL: https://www.microsoft.com/en-us/download/details.aspx?id=8109

### Visual Studio
Once the project has been opened, ensure that DirectX SDK June 2010 is being used by making the small changes below.

Navigate to the VC++ Directories configuration by doing the following: Project -> Properties -> VC++ Directories. Ensure that both the Include Directories and Library Directories point to the folder created by the DirectX SDK June 2010, and the respective include and directory folders.

Eg.
- Include Directories: C:\Program Files x86\Microsoft DirectX SDK June 2010\Include
- Library Directories: C:\Program Files x86\Microsoft DirectX SDK June 2010\Lib\x86
