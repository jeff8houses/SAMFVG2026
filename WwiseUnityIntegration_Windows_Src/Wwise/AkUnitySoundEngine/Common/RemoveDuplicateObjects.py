import subprocess
import os
import sys

if(len(sys.argv) < 2):
    raise RuntimeError("Error: please specify library to strip as first argument")

sourceLib = sys.argv[1]
libName = os.path.basename(sourceLib)
libPath = os.path.dirname(sourceLib)

if "simulator" not in sourceLib:
    print("No need to remove duplicate symbols on non-simulator targets.")
    sys.exit()

print("------ Removing duplicate symbols from " + libName + " at path " + libPath)

# Get all arches in the lib
cmd = ['lipo', sourceLib, '-info']
process = subprocess.run(cmd, capture_output=True, text=True)
arches = process.stdout[process.stdout.rfind(': ')+2:].strip().split(' ')

outputLibs = []
for arch in arches:
    tempFolder = os.path.join(os.path.dirname(__file__), "temp", arch)
    if not os.path.exists(tempFolder):
        os.makedirs(tempFolder)

    # Extract the arch from the fat binary
    outputLib = os.path.join(tempFolder, libName)
    cmd = ['lipo', sourceLib, '-thin', arch, '-output', outputLib]
    subprocess.run(cmd)

    # Get the symbols from the extacted thin library
    cmd = ['ar', 't', outputLib]
    arProcess = subprocess.run(cmd, capture_output=True, text=True)
    objList = arProcess.stdout.split('\n')
    objList.sort()

    # Remove duplicate objects. They share the same name, but with the last character
    # in their name incremented by one. For example, 'stdafx', 'stdafy', 'stdafz', etc.
    dupes = set()
    i = 0
    while i < len(objList):
        if objList[i]:
            obj = objList[i]
            commonRoot = obj.removesuffix(".o")
            commonRoot = commonRoot[:len(commonRoot)-1]
            j = i+1
            while j < len(objList) and objList[j].startswith(commonRoot):
                if len(obj) == len(objList[j]):
                    dupes.add(objList[j])
                j += 1
            i += 1
        else:
            i += 1

    outputLibs.append(outputLib)
    cmd = ['ar', 'd', outputLib] + list(dupes)
    subprocess.run(cmd)

# Re-create the fat binary
cmd = ['libtool', '-o', sourceLib, '-static'] + outputLibs
subprocess.run(cmd)


