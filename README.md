# Vertexing for Scouting muons in hltpackerVtx collection

This code performs vertexing for Scouting muons in Vtx collections. It performs a fit of the track of the muons and find the x, y and z position of the secondary vertex

## HOW TO RUN:
1. Set up a CMSSW release:
```bash
cmsrel CMSSW_15_1_0
cd CMSSW_15_1_0/src
cmsenv
```
2. Set up the correct directories and clone git repo:
```bash
mkdir sct_vertex
cd sct_vertex
git clone https://github.com/JavierGarciadeCastro/Scouting_vertexing.git
cd Scouting_vertexing
```
3. Compile the code:
```bash
scram b -j 8
```
4. Run the test:
```bash
cmsRun test/run_vertexing.py
```