#pragma once
#include <fstream>
#include <map>
#include <string>
#include "Spline.h"
#include <cereal/archives/binary.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

struct ArchiveSplines {
	std::map<std::string, Spline> limitFactorCurves;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(limitFactorCurves);
	}
};

void saveCurves(const ArchiveSplines& curves, const std::string& fileName) {
    std::ofstream os(fileName, std::ios::binary);
    cereal::BinaryOutputArchive archive(os);
    archive(curves);
}

ArchiveSplines loadCurves(const std::string& fileName) {
    std::ifstream is(fileName, std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    ArchiveSplines curves;
    archive(curves);
    return curves;
}

