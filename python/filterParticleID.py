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

    dataFrame2.Snapshot("tree", "test.root", VARIABLE_LIST)
