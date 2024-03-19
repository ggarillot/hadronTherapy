#include "CLI11.hpp"

#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <Rtypes.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>

#include <filesystem>
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <string>

int main(int argc, char** argv)
{
    namespace fs = std::filesystem;

    CLI::App app;

    std::string fileName{};
    std::string timeBeginStr{};
    std::string timeEndStr{};
    std::string irrTimeStr{};
    std::string precisionStr{};

    int nBins{};

    double nIrrad{};
    bool   noPlotHisto = false;

    app.add_option("-f", fileName, "fileName")->required()->check(CLI::ExistingFile);
    app.add_option("-b", timeBeginStr, "time begin (min)")->default_val("0");
    app.add_option("-e", timeEndStr, "time end (min)")->default_val("0");
    app.add_option("-r", irrTimeStr, "irradiation time (min)")->default_val("0");
    app.add_option("-n", nIrrad, "n hadrons irradiation")->default_val(1e9);
    app.add_option("-p", precisionStr, "detector precision (mm)")->default_val("0");
    app.add_option("-B", nBins, "nBins histogram")->default_val(120);
    app.add_flag("--noHisto", noPlotHisto, "do not plot dose histo");

    CLI11_PARSE(app, argc, argv);

    double timeBegin = std::stod(timeBeginStr);
    double timeEnd = std::stod(timeEndStr);

    if (timeBegin < 0)
        timeBegin = 0;
    if (timeEnd <= timeBegin)
        timeEnd = 2 * 60;

    double precision = std::stod(precisionStr);

    gStyle->SetOptStat(0);
    ROOT::EnableImplicitMT();

    const auto fileStem = fs::path{fileName}.stem().string();

    auto inputFile = TFile::Open(fileName.c_str(), "READ");
    auto inputTree = dynamic_cast<TTree*>(inputFile->Get("tree"));

    const auto histoDose = dynamic_cast<TH1D*>(inputFile->Get("histo"));
    histoDose->SetLineWidth(3);
    histoDose->SetLineStyle(2);
    histoDose->SetLineColor(kRed);

    const auto braggPeakDepth = histoDose->GetBinCenter(histoDose->GetMaximumBin());

    if (!inputTree)
    {
        std::cout << "ERROR : no tree" << std::endl;
        return 1;
    }

    auto inputDF = ROOT::RDataFrame{*inputTree};

    const ULong64_t nEvents = inputDF.Count().GetValue();
    auto            irrTime = std::stod(irrTimeStr);

    std::cout << "nHadrons in file = " << nEvents << '\n'
              << "irradiationTime = " << irrTime << " minutes" << '\n'
              << "nHadrons target = " << nIrrad << '\n'
              << "measure from " << timeBegin << " to " << timeEnd << " minutes after irradiation" << '\n'
              << "detector precision = " << precision << " mm" << std::endl;

    timeBegin *= 60;
    timeEnd *= 60;
    irrTime *= 60;

    auto data = inputDF.Define(
        "eventTime", [&](const ULong64_t& event) { return irrTime * ((1. * event / nEvents) - 1); }, {"rdfentry_"});

    auto timeFilter = [&](const double& eventTime, const ROOT::VecOps::RVec<double>& time) -> ROOT::VecOps::RVec<bool>
    { return ((eventTime + time) > timeBegin) && ((eventTime + time) < timeEnd); };

    auto O_Filter = [&](const ROOT::VecOps::RVec<int>& id) -> ROOT::VecOps::RVec<bool> { return id == 8; };
    auto C_Filter = [&](const ROOT::VecOps::RVec<int>& id) -> ROOT::VecOps::RVec<bool> { return id == 6; };
    auto N_Filter = [&](const ROOT::VecOps::RVec<int>& id) -> ROOT::VecOps::RVec<bool> { return id == 7; };

    data = data.Define("maskT", timeFilter, {"eventTime", "t"});

    auto maskFuncI = [](const ROOT::RVec<int>& vec, const ROOT::RVec<bool>& mask) { return vec[mask]; };
    auto maskFuncD = [](const ROOT::RVec<double>& vec, const ROOT::RVec<bool>& mask) { return vec[mask]; };

    auto dataTimeFiltered =
        data.Define("zTimeFilter", maskFuncD, {"z", "maskT"}).Define("idTimeFilter", maskFuncI, {"id", "maskT"});

    dataTimeFiltered = dataTimeFiltered.Define("maskO", O_Filter, {"idTimeFilter"});
    dataTimeFiltered = dataTimeFiltered.Define("maskC", C_Filter, {"idTimeFilter"});
    dataTimeFiltered = dataTimeFiltered.Define("maskN", N_Filter, {"idTimeFilter"});

    unsigned long long nCalled = 0;

    auto simulPrecision = [&](const ROOT::VecOps::RVec<double>& z)
    {
        std::random_device         rd{};
        std::mt19937               gen{rd()};
        std::normal_distribution<> d{0, precision / 2.355};

        auto zDetected = z;

        for (auto& val : zDetected)
            val += d(gen);

        nCalled++;

        return zDetected;
    };

    if (precision < std::numeric_limits<double>::epsilon())
        dataTimeFiltered = dataTimeFiltered.Define("zDetected", "zTimeFilter");
    else
        dataTimeFiltered = dataTimeFiltered.Define("zDetected", simulPrecision, {"zTimeFilter"});

    std::cout << "before mean " << nCalled << std::endl;

    // std::cout << dataTimeFiltered.Mean("zDetected").GetValue() << std::endl;

    dataTimeFiltered = dataTimeFiltered.Define("zDetectedO", maskFuncD, {"zDetected", "maskO"})
                           .Define("zDetectedC", maskFuncD, {"zDetected", "maskC"})
                           .Define("zDetectedN", maskFuncD, {"zDetected", "maskN"});

    const ROOT::RDF::TH1DModel model = {"", ";depth [mm]; Activity [arbitrary units]", nBins, 0, braggPeakDepth * 1.4};

    auto histoAll = dataTimeFiltered.Histo1D(model, {"zDetected"});
    auto histoO = dataTimeFiltered.Histo1D(model, {"zDetectedO"});
    auto histoC = dataTimeFiltered.Histo1D(model, {"zDetectedC"});
    auto histoN = dataTimeFiltered.Histo1D(model, {"zDetectedN"});

    std::cout << "before histoAll " << nCalled << std::endl;
    ROOT::RDF::RunGraphs({histoAll, histoC, histoN, histoO});
    std::cout << "histos done  " << nCalled << std::endl;

    histoAll->SetLineColor(kBlack);
    histoO->SetLineColor(kBlue - 3);
    histoC->SetLineColor(kMagenta);
    histoN->SetLineColor(kGreen + 2);

    histoAll->SetLineWidth(2);
    histoO->SetLineWidth(2);
    histoC->SetLineWidth(2);
    histoN->SetLineWidth(2);

    auto canvas = new TCanvas("c", "c", 1200, 800);
    canvas->SetTicks();
    canvas->SetRightMargin(0.025);
    canvas->SetTopMargin(0.035);

    // const auto scalingFactor = 1. / histoAll->Integral();
    const auto scalingFactor = nIrrad / nEvents * (60 * (timeEnd - timeBegin));

    histoAll->Scale(scalingFactor);
    histoO->Scale(scalingFactor);
    histoC->Scale(scalingFactor);
    histoN->Scale(scalingFactor);

    const auto maxAll = histoAll->GetBinContent(histoAll->GetMaximumBin());
    const auto maxDose = histoDose->GetBinContent(histoDose->GetMaximumBin());

    histoAll->Draw("hist");
    histoO->Draw("hist same");
    histoC->Draw("hist same");
    histoN->Draw("hist same");

    if (!noPlotHisto)
    {
        histoDose->Scale(maxAll / maxDose);
        histoDose->Draw("hist same");
    }

    auto legend = TLegend(0.75, 0.6, 0.95, 0.85);
    legend.SetBorderSize(0);

    const auto legHeader = "t = " + timeBeginStr + " min to t = " + timeEndStr + " min";
    legend.SetHeader(legHeader.c_str());
    legend.SetTextFont(62);

    auto entryO = legend.AddEntry(histoO.GetPtr(), "{}^{15}O", "l");
    auto entryC = legend.AddEntry(histoC.GetPtr(), "{}^{11}C", "l");
    auto entryN = legend.AddEntry(histoN.GetPtr(), "{}^{13}N", "l");
    auto entryAll = legend.AddEntry(histoAll.GetPtr(), "Sum", "l");

    entryO->SetTextFont(42);
    entryC->SetTextFont(42);
    entryN->SetTextFont(42);
    entryAll->SetTextFont(42);

    if (!noPlotHisto)
    {
        auto entryDose = legend.AddEntry(histoDose, "dose", "l");
        entryDose->SetTextFont(42);
    }

    legend.Draw("same");

    const auto outputFileName =
        fileStem + "_activity_[" + timeBeginStr + "," + timeEndStr + "]_p" + precisionStr + "_i" + irrTimeStr + ".root";

    auto outputFile = TFile::Open(outputFileName.c_str(), "RECREATE");
    histoAll->Write("histoAll");
    histoO->Write("histoO");
    histoC->Write("histoC");
    histoN->Write("histoN");
    histoDose->Write("histoDose");
    canvas->Write("c");
    // canvas->SaveAs(outputFileName.c_str());

    outputFile->Close();
    inputFile->Close();

    return 0;
}