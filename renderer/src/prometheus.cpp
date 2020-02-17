//
// Created by rewbycraft on 2/16/20.
//

#include "sixelping/renderer/prometheus.h"
#include <sixelping/renderer/options.h>
#include <cpr/cpr.h>
#include <rapidjson/document.h>
#include <sixelping/logs.h>

namespace sixelping::renderer::prometheus {
	std::optional<double> query(std::string query) {
		auto logger = sixelping::get_or_create_logger("prom-client");
		const auto &opts = sixelping::renderer::options::get();
		
		if (opts.prom.empty())
			return std::nullopt;
		
		auto r = cpr::Get(cpr::Url{opts.prom + "/api/v1/query"},
		                  cpr::Parameters{{"query", query}},
		                  cpr::Header{{"Accept", "*/*"}},
		                  cpr::VerifySsl(false),
		                  cpr::Verbose(false));
		
		if (r.status_code != 200) {
			logger->warn("Prometheus error: {}", r.status_code);
			logger->warn("Error message: {}", r.error.message);
			return std::nullopt;
		}
		
		rapidjson::Document document;
		document.Parse(r.text.c_str(), r.text.size());
		
		if (document.HasParseError()) {
			return std::nullopt;
		}
		
		auto &results = document["data"]["result"];
		
		for (auto &result : results.GetArray()) {
			return {std::stod(result["value"][1].GetString())};
		}
		
		return std::nullopt;
	}
}
