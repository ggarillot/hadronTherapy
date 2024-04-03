import ROOT as R
import argparse

if __name__ == "__main__":

    VARIABLE_LIST = [
        "pdgEsc",
        "xEsc",
        "yEsc",
        "zEsc",
        "thetaEsc",
        "phiEsc",
        "eEsc",
        "timeEsc",
        "initialXEsc",
        "initialYEsc",
        "initialZEsc",
    ]

    parser = argparse.ArgumentParser(prog="filterByParticle", description="")

    parser.add_argument("-f", "--file", required=True)
    parser.add_argument("-p", "--particle", required=True)

    args = parser.parse_args()

    PARTICLE_ID = args.particle
    INPUTFILE = args.file

    dataFrame = R.RDataFrame("tree", INPUTFILE)

    dataFrame2 = dataFrame.Define("cut", f"pdgEsc=={PARTICLE_ID}")

    for variable in VARIABLE_LIST:
        dataFrame2 = dataFrame2.Redefine(f"{variable}", f"{variable}[cut]")

    split = INPUTFILE.split(".")[0]
    split = f"{split}_{PARTICLE_ID}.root"
    dataFrame2.Snapshot("tree", split, VARIABLE_LIST)

    inputFile = R.TFile.Open(INPUTFILE, "READ")
    outputFile = R.TFile.Open(split, "UPDATE")

    histo = inputFile.Get("histo")

    outputFile.cd()
    histo.Write()
    outputFile.Close()
    inputFile.Close()
