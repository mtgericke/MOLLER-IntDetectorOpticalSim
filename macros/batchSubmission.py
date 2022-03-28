#!/usr/bin/env python

import sys
import os
import subprocess
import math
import time
import argparse
import numpy as np

parser = argparse.ArgumentParser(description="Submit array jobs to ifarm.")
parser.add_argument("-c", dest="cluster", action="store", required=True, help="Enter the Server Name. Example: ifarm,grex,beluga")
parser.add_argument("-a", dest="account", action="store", required=True, help="Enter the Jefferson Lab account. Example: halla")  
parser.add_argument("-s", dest="src", action="store", required=False, default="/w/halla-scshelf2102/moller12gev/rahmans/mollerOpticalSim", help="source folder where simulation directory exists")
parser.add_argument("-v", dest="version", action="store", required=False, default="MOLLER-IntDetectorOpticalSim", help= "choose the version of simulation to use. Should just be the name of the repository unless you rename local folder")
parser.add_argument("-e", dest="eic_shell", action="store", required=False, default="/w/halla-scshelf2102/moller12gev/rahmans/mollerOpticalSim/eic-shell", help= "provide the location of eic-shell")
parser.add_argument("-j", dest="jsub_dir", action="store", required=True, help="choose directory to write the slurm submission scripts")
parser.add_argument("-t", dest="tmp_dir", action="store", required=True, help="choose directory to write the slurm output logs")
parser.add_argument("-o", dest="out_dir", action="store", required=True, help="choose where to write the output root files")
parser.add_argument("-r", dest="run_range", action = "store", required=False, default="1", help="provide run range. Example: \"2-5\"")
parser.add_argument("-n", dest="n_events", action= "store", required=False, default=1000, help= "provide number of events per job in the array")
parser.add_argument("--time", dest="time", action= "store", required= False, default= "00:25:00", help= "provide the estimated run time. Ex: \"00:25:00\". Usually it is 10 minutes for 1000 moller events.")

args=parser.parse_args()

args.eic_shell=os.path.realpath(args.eic_shell)
args.src=os.path.realpath(args.src)

if not os.path.exists(args.jsub_dir):
        os.system("mkdir -p "+args.jsub_dir)
if not os.path.exists(args.tmp_dir):
        os.system("mkdir -p "+args.tmp_dir)
if not os.path.exists(args.out_dir):
        os.system("mkdir -p "+args.out_dir)

args.tmp_dir=os.path.realpath(args.tmp_dir)
args.jsub_dir=os.path.realpath(args.jsub_dir)
out=os.path.realpath(args.out_dir)
x=args.run_range
index=x.rfind("-")+1

EventHitRegion = str(4)
LightGuideLowerConeBackAngle = list(map(str, np.random.uniform(0,45,size=1+int(x[index:])-int(x[:index-1]))))
LightGuideLowerConeFrontAngle = list(map(str, np.random.uniform(0,45,size=1+int(x[index:])-int(x[:index-1]))))
LightGuideLowerInterface = list(map(str, np.random.uniform(2,100,size=1+int(x[index:])-int(x[:index-1]))))
LightGuideUpperInterface = list(map(str, np.random.uniform(500,700,size=1+int(x[index:])-int(x[:index-1]))))
LightGuidePMTInterfaceOpeningX = list(map(str, np.random.uniform(50,90, size=1+int(x[index:])-int(x[:index-1]))))
LightGuidePMTInterfaceOpeningZ = list(map(str, np.random.uniform(50,90, size=1+int(x[index:])-int(x[:index-1]))))
LightGuideQuartzInterfaceOpeningX = list(map(str, np.random.uniform(20,200, size=1+int(x[index:])-int(x[:index-1]))))
LightGuideQuartzInterfaceOpeningZ = list(map(str, np.random.uniform(20,200, size=1+int(x[index:])-int(x[:index-1]))))
QuartzSizeZ = list(map(str, np.random.uniform(5,25,size=1+int(x[index:])-int(x[:index-1]))))
QuartzSizeX = list(map(str, np.random.uniform(120,220,size=1+int(x[index:])-int(x[:index-1]))))
QuartzSizeY = list(map(str, np.random.uniform(55,65,size=1+int(x[index:])-int(x[:index-1]))))
QuartzBevelSize = list(map(str, np.random.uniform(0.05,0.11,size=(1+int(x[index:])-int(x[:index-1])))))
QuartzRotX = list(map(str, np.random.uniform(-3,3,size=(1+int(x[index:])-int(x[:index-1])))))
PolarRotation = list(map(str, np.random.uniform(-3,3,size=(1+int(x[index:])-int(x[:index-1])))))
LightGuideQuartzToPMTOffset = list(map(str, np.random.uniform(-0.001,0.001,size=(1+int(x[index:])-int(x[:index-1])))))
		
jsubf=open(args.jsub_dir+"/run.sh", "w")
jsubf.write("#!/bin/bash\n")
jsubf.write("#SBATCH --account="+args.account+"\n")
if (args.cluster == "ifarm"):
  jsubf.write("#SBATCH --partition=production\n")
if (args.cluster == "grex"):
  jsubf.write("#SBATCH --partition=compute\n")
jsubf.write("#SBATCH --job-name=molleropt\n")
jsubf.write("#SBATCH --time="+args.time+" \n")
jsubf.write("#SBATCH --nodes=1\n")
jsubf.write("#SBATCH --ntasks=1\n")
jsubf.write("#SBATCH --cpus-per-task=1\n")
jsubf.write("#SBATCH --mem=4G\n")
jsubf.write("#SBATCH --output="+args.tmp_dir+"/run_%A_%a.out\n")

for i in range(int(x[:index-1]),1+int(x[index:])):
  jsubf.write("LightGuideLowerConeBackAngle["+str(i)+"]="+LightGuideLowerConeBackAngle[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuideLowerConeFrontAngle["+str(i)+"]="+LightGuideLowerConeFrontAngle[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuideLowerInterface["+str(i)+"]="+LightGuideLowerInterface[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuideUpperInterface["+str(i)+"]="+LightGuideUpperInterface[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuidePMTInterfaceOpeningX["+str(i)+"]="+LightGuidePMTInterfaceOpeningX[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuidePMTInterfaceOpeningZ["+str(i)+"]="+LightGuidePMTInterfaceOpeningZ[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuideQuartzInterfaceOpeningX["+str(i)+"]="+LightGuideQuartzInterfaceOpeningX[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuideQuartzInterfaceOpeningZ["+str(i)+"]="+LightGuideQuartzInterfaceOpeningZ[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("QuartzSizeZ["+str(i)+"]="+QuartzSizeZ[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("QuartzSizeX["+str(i)+"]="+QuartzSizeX[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("QuartzSizeY["+str(i)+"]="+QuartzSizeY[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("QuartzBevelSize["+str(i)+"]="+QuartzBevelSize[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("QuartzRotX["+str(i)+"]="+QuartzRotX[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("PolarRotation["+str(i)+"]="+PolarRotation[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")
  jsubf.write("LightGuideQuartzToPMTOffset["+str(i)+"]="+LightGuideQuartzToPMTOffset[i-(1+int(x[index:])-int(x[:index-1]))]+"\n")

if (args.cluster == "ifarm"):
  jsubf.write("cd /scratch/slurm\n")
  jsubf.write("cd ${SLURM_JOB_ID}\n")
if (args.cluster == "grex"):
  jsubf.write("cd /scratch\n")
if (args.cluster == "beluga" or args.cluster == "narval"):
  jsubf.write("cd $TMPDIR\n")
jsubf.write("mkdir ${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}\n")
jsubf.write("cd ${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}\n")
macro="run.mac"
jsubf.write("touch "+macro+"\n")
jsubf.write("echo /Det/LightGuideLowerConeBackAngle ${LightGuideLowerConeBackAngle[${SLURM_ARRAY_TASK_ID}]} deg >>"+macro+"\n")
jsubf.write("echo /Det/LightGuideLowerConeFrontAngle ${LightGuideLowerConeFrontAngle[${SLURM_ARRAY_TASK_ID}]} deg>>"+macro+"\n")
jsubf.write("echo /Det/LightGuideLowerInterface ${LightGuideLowerInterface[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/LightGuideUpperInterface ${LightGuideUpperInterface[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/LightGuidePMTInterfaceOpeningX ${LightGuidePMTInterfaceOpeningX[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/LightGuidePMTInterfaceOpeningZ ${LightGuidePMTInterfaceOpeningZ[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/LightGuideQuartzInterfaceOpeningX ${LightGuideQuartzInterfaceOpeningX[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/LightGuideQuartzInterfaceOpeningZ ${LightGuideQuartzInterfaceOpeningZ[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/QuartzSizeZ ${QuartzSizeZ[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/QuartzSizeX ${QuartzSizeX[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/QuartzSizeY ${QuartzSizeY[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/QuartzBevelSize ${QuartzBevelSize[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/QuartzRotX ${QuartzRotX[${SLURM_ARRAY_TASK_ID}]} deg >>"+macro+"\n")
jsubf.write("echo /Det/PolarRotation ${PolarRotation[${SLURM_ARRAY_TASK_ID}]} deg >>"+macro+"\n")
jsubf.write("echo /Det/LightGuideQuartzToPMTOffset ${LightGuideQuartzToPMTOffset[${SLURM_ARRAY_TASK_ID}]} mm >>"+macro+"\n")
jsubf.write("echo /Det/UpdateGeometry >>"+macro+"\n")
jsubf.write("echo /Generator/EventHitRegion "+EventHitRegion+" >>"+macro+"\n")
jsubf.write("echo /RunAction/SetID ${SLURM_ARRAY_TASK_ID} >>"+macro+"\n")
jsubf.write("echo /random/setSeeds ${SLURM_ARRAY_JOB_ID}${SLURM_ARRAY_TASK_ID} ${SLURM_ARRAY_TASK_ID}${SLURM_ARRAY_JOB_ID} >>"+macro+"\n")
jsubf.write("echo /run/beamOn "+str(args.n_events)+" >>"+macro+"\n")
jsubf.write("cat "+macro+"\n")
jsubf.write("cp -r "+args.src+"/"+args.version+" .\n")
jsubf.write("cd "+args.version+"/build \n")
jsubf.write("echo \"Current working directory is `pwd`\"\n")
jsubf.write("cat << EOF | "+args.eic_shell+"\n")
if (args.cluster == "ifarm"):
  jsubf.write("./MOLLEROpt /scratch/slurm/${SLURM_JOB_ID}/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}/run.mac\n")
if (args.cluster == "grex"):
  jsubf.write("./MOLLEROpt /scratch/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}/run.mac\n")
if (args.cluster == "beluga" or args.cluster == "narval"):
  jsubf.write("./MOLLEROpt $TMPDIR/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}/run.mac\n")
jsubf.write("EOF\n")
jsubf.write("echo \"Program remoll finished with exit code $? at: `date`\"\n")
jsubf.write("cp *.root "+out+"\n")
if (args.cluster == "ifarm"):
  jsubf.write("rm -rf /scratch/slurm/${SLURM_JOB_ID}/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}\n")
if (args.cluster == "grex"):
  jsubf.write("rm -rf /scratch/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}\n")
if (args.cluster == "beluga" or args.cluster == "narval"):
  jsubf.write("rm -rf $TMPDIR/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}\n")
jsubf.close()
	        
                
subprocess.call("sbatch --array="+args.run_range+" "+args.jsub_dir+"/run.sh",shell=True)
