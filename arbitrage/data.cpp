#include <iostream>
#include <iomanip>
#include "data.h"

Data::Data(float ml, float mv) {
	this->minLiquidity = ml;
	this->minVolume = mv;
}

void Data::getData() {
	beginPrint();
	std::string url = "https://coinmarketcap.com/";
	std::string stringToFind = "\"id\":";
	std::string homeHTML = CurlObj(url).getData();
	int homePageIndex = homeHTML.find(stringToFind, 0) + stringToFind.length();
	int marketPageIndex = 0;
	int currencyID = 0;
	int i = 0;
	while (homePageIndex > stringToFind.length()) {
		if (!isdigit(homeHTML.at(homePageIndex)))
			goto reset;
		currencyID = std::stoi(homeHTML.substr(homePageIndex));
		url += "currencies/";
		stringToFind = "\"slug\":\"";
		homePageIndex = homeHTML.find(stringToFind, homePageIndex) + stringToFind.length();
		for (int j = 0; homeHTML.at(homePageIndex + j) != '"'; ++j) {
			url += (homeHTML.at(homePageIndex + j) == ' ') ? '-' : std::tolower(homeHTML.at(homePageIndex + j));
		}
		url += "/markets/";
		this->html = CurlObj(url).getData();
		marketPageIndex = getHighestIndex(currencyID);
		if (!marketPageIndex)
			goto reset;
		stringToFind = "\"slug\":\"";
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		this->trades[i].buyExchange = this->html.substr(marketPageIndex, this->html.find('"', marketPageIndex) - marketPageIndex);
		stringToFind = "\"currency_symbol\":\"";
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		for (int j = 0; this->html.at(marketPageIndex + j) != '"'; ++j) {
			this->trades[i].buyPair += this->html.at(marketPageIndex + j);
		}
		this->trades[i].buyPair += '/';
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		for (int j = 0; this->html.at(marketPageIndex + j) != '"'; ++j) {
			this->trades[i].buyPair += this->html.at(marketPageIndex + j);
		}
		stringToFind = "\"USD\":{\"price\":";
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		this->trades[i].buyPrice = std::stof(this->html.substr(marketPageIndex));
		marketPageIndex = getLowestIndex(currencyID);
		if (!marketPageIndex)
			goto reset;
		stringToFind = "\"slug\":\"";
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		this->trades[i].sellExchange = this->html.substr(marketPageIndex, this->html.find('"', marketPageIndex) - marketPageIndex);
		stringToFind = "\"currency_symbol\":\"";
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		for (int j = 0; this->html.at(marketPageIndex + j) != '"'; ++j) {
			this->trades[i].sellPair += this->html.at(marketPageIndex + j);
		}
		this->trades[i].sellPair += '/';
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		for (int j = 0; this->html.at(marketPageIndex + j) != '"'; ++j) {
			this->trades[i].sellPair += this->html.at(marketPageIndex + j);
		}
		stringToFind = "\"USD\":{\"price\":";
		marketPageIndex = this->html.find(stringToFind, marketPageIndex) + stringToFind.length();
		this->trades[i].sellPrice = std::stof(this->html.substr(marketPageIndex));
		this->trades[i].profitRatio = this->trades[i].buyPrice / this->trades[i].sellPrice;
		printData(this->trades[i++]);
	reset:
		currencyID = 0;
		url = "https://coinmarketcap.com/";
		stringToFind = "\"id\":";
		homePageIndex = homeHTML.find(stringToFind, homePageIndex) + stringToFind.length();
	}
}

int Data::getHighestIndex(int id) {
	std::string stringToFind = "\"exchange\":{\"id\":";
	float highest = 0;
	float temp = 0;
	int highestIndex = 0;
	int dummyIndex = 0;
	int index = this->html.find(stringToFind) + stringToFind.length();
	while(index > stringToFind.length()) {
		stringToFind = "\"outlier_detected\":";
		if (this->html.at(this->html.find(stringToFind, index) + stringToFind.length()) - '0')
			goto reset;
		stringToFind = "\"effective_liquidity\":";
		if (std::stof(this->html.substr(this->html.find(stringToFind, index) + stringToFind.length())) < minLiquidity)
			goto reset;
		stringToFind = "\"volume_24h_base\":";
		if (std::stof(this->html.substr(this->html.find(stringToFind, index) + stringToFind.length())) < minVolume)
			goto reset;
		stringToFind = "\"currency_id\":";
		dummyIndex = this->html.find(stringToFind, index) + stringToFind.length();
		temp = std::stof(this->html.substr(dummyIndex));
		if (temp == id) {
			temp = 0;
			stringToFind = "\"USD\":{\"price\":";
			dummyIndex = this->html.find(stringToFind, index) + stringToFind.length();
			temp = std::stof(this->html.substr(dummyIndex));
			if (temp > highest) {
				highest = temp;
				highestIndex = index;
			}
		}
	reset:
		stringToFind = "\"exchange\":{\"id\":";
		index = this->html.find(stringToFind, index) + stringToFind.length();
		temp = 0;
	}
	return highestIndex;
}

int Data::getLowestIndex(int id) {
	std::string stringToFind = "\"exchange\":{\"id\":";
	float lowest = INFINITY;
	float temp = 0;
	int lowestIndex = 0;
	int dummyIndex = 0;
	int index = this->html.find(stringToFind) + stringToFind.length();
	while (index > stringToFind.length()) {
		stringToFind = "\"outlier_detected\":";
		if (this->html.at(this->html.find(stringToFind, index) + stringToFind.length()) - '0')
			goto reset;
		stringToFind = "\"effective_liquidity\":";
		if (std::stof(this->html.substr(this->html.find(stringToFind, index) + stringToFind.length())) < minLiquidity)
			goto reset;
		stringToFind = "\"volume_24h_base\":";
		if (std::stof(this->html.substr(this->html.find(stringToFind, index) + stringToFind.length())) < minVolume)
			goto reset;
		stringToFind = "\"currency_id\":";
		dummyIndex = this->html.find(stringToFind, index) + stringToFind.length();
		temp = std::stof(this->html.substr(dummyIndex));
		if (temp == id) {
			temp = 0;
			stringToFind = "\"USD\":{\"price\":";
			dummyIndex = this->html.find(stringToFind, index) + stringToFind.length();
			temp = std::stof(this->html.substr(dummyIndex));
			if (temp < lowest) {
				lowest = temp;
				lowestIndex = index;
			}
		}
	reset:
		stringToFind = "\"exchange\":{\"id\":";
		index = this->html.find(stringToFind, index) + stringToFind.length();
		temp = 0;
	}
	return lowestIndex;
}

void Data::beginPrint() {
	std::cout
		<< std::setw(10) << "BUY PAIR" << "\t"
		<< std::setw(10) << "SELL PAIR" << "\t"
		<< std::setw(20) << "BUY EXCHANGE" << "\t"
		<< std::setw(20) << "SELL EXCHANGE" << "\t"
		<< std::setw(10) << "BUY PRICE" << "\t"
		<< std::setw(10) << "SELL PRICE" << "\t"
		<< std::setw(10) << "RATIO" << "\n";
}

void Data::printData(struct arbitrageData ad) {
	std::cout 
		<< std::setw(10) << ad.buyPair << "\t"
		<< std::setw(10) << ad.sellPair << "\t"
		<< std::setw(20) << ad.buyExchange <<"\t"
		<< std::setw(20) << ad.sellExchange << "\t"
		<< std::setw(10) << ad.buyPrice << "\t"
		<< std::setw(10) << ad.sellPrice << "\t"
		<< std::setw(10) << ad.profitRatio << "\n";
}