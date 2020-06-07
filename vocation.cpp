/**
 * The Forgotten Server - a free and open-source MMORPG server emulator
 * Copyright (C) 2019  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "otpch.h"

#include "vocation.h"

#include "pugicast.h"
#include "tools.h"

#include "player.h"
#include "game.h"

#include "luascript.h"

bool Vocations::loadFromXml()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("data/XML/vocations.xml");
	if (!result) {
		printXMLError("Error - Vocations::loadFromXml", "data/XML/vocations.xml", result);
		return false;
	}

	for (auto vocationNode : doc.child("vocations").children()) {
		pugi::xml_attribute attr;
		if (!(attr = vocationNode.attribute("id"))) {
			std::cout << "[Warning - Vocations::loadFromXml] Missing vocation id" << std::endl;
			continue;
		}

		uint16_t id = pugi::cast<uint16_t>(attr.value());

		auto res = vocationsMap.emplace(std::piecewise_construct,
				std::forward_as_tuple(id), std::forward_as_tuple(id));
		Vocation& voc = res.first->second;

		if ((attr = vocationNode.attribute("name"))) {
			voc.name = attr.as_string();
		}

		if ((attr = vocationNode.attribute("clientid"))) {
			voc.clientId = pugi::cast<uint16_t>(attr.value());
		}

		if ((attr = vocationNode.attribute("description"))) {
			voc.description = attr.as_string();
		}

		voc.gainCap = 10 * 100;

		if ((attr = vocationNode.attribute("gainhp"))) {
			voc.gainHP = pugi::cast<uint32_t>(attr.value());
		}

		if ((attr = vocationNode.attribute("gainmana"))) {
			voc.gainMana = pugi::cast<uint32_t>(attr.value());
		}

		voc.gainHealthTicks = 6;

		voc.gainHealthAmount = 5000;

		voc.gainManaTicks = 3;

		voc.gainManaAmount = 5000;;

		voc.manaMultiplier = 1.1;

		voc.attackSpeed = 1300;

		voc.baseSpeed = 100;

		voc.soulMax = 100;

		voc.gainSoulTicks = 120;
		
		if ((attr = vocationNode.attribute("weapon"))) {
			voc.weapon = attr.as_string();
		}
		
		if ((attr = vocationNode.attribute("looktype"))) {
			voc.looktype = pugi::cast<uint32_t>(attr.value());
		}

		if ((attr = vocationNode.attribute("fromvoc"))) {
			voc.fromVocation = pugi::cast<uint32_t>(attr.value());
		}
		
		voc.skillMultipliers[0] = 1.2;
		voc.skillMultipliers[1] = 1.1;
		if (voc.weapon == "glove") {
			voc.skillMultipliers[2] = 2.0;
			voc.skillMultipliers[3] = 1.1;
			voc.skillMultipliers[4] = 2.0;
		} else if (voc.weapon == "sword") {
			voc.skillMultipliers[2] = 1.1;
			voc.skillMultipliers[3] = 2.0;
			voc.skillMultipliers[4] = 2.0;
		} else if (voc.weapon == "distance") {
			voc.skillMultipliers[2] = 2.0;
			voc.skillMultipliers[3] = 2.0;
			voc.skillMultipliers[4] = 1.1;
		}
		
		voc.meleeDamageMultiplier = 1.0f;
		voc.distDamageMultiplier = 1.0f;
		voc.defenseMultiplier = 1.0f;
		voc.armorMultiplier = 1.0f;

		for (auto childNode : vocationNode.children()) {
			if (strcasecmp(childNode.name(), "transform") == 0) {
				pugi::xml_attribute transformAttribute = childNode.attribute("id");

				pugi::xml_attribute levelReqAttribute = childNode.attribute("levelReq");
				if (levelReqAttribute) {
					voc.levelReq[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<uint64_t>(levelReqAttribute.value());
				}

				pugi::xml_attribute newLooktypeAttribute = childNode.attribute("newLooktype");
				if (newLooktypeAttribute) {
					voc.newLooktype[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<uint64_t>(newLooktypeAttribute.value());
				}

				pugi::xml_attribute meleeDamageMultiplierTAttribute = childNode.attribute("meleeDamageT");
				if (meleeDamageMultiplierTAttribute) {
					voc.meleeDamageMultiplierT[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<float>(meleeDamageMultiplierTAttribute.value());
				}

				pugi::xml_attribute distDamageMultiplierTAttribute = childNode.attribute("distDamageMultiplierT");
				if (distDamageMultiplierTAttribute) {
					voc.distDamageMultiplierT[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<float>(distDamageMultiplierTAttribute.value());
				}

				pugi::xml_attribute additionalHealthAttribute = childNode.attribute("additionalHealth");
				if (additionalHealthAttribute) {
					voc.additionalHealth[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<uint64_t>(additionalHealthAttribute.value());
				}

				pugi::xml_attribute additionalManaAttribute = childNode.attribute("additionalMana");
				if (additionalManaAttribute) {
					voc.additionalMana[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<uint64_t>(additionalManaAttribute.value());
				}

				pugi::xml_attribute effectAttribute = childNode.attribute("effect");
				if (effectAttribute) {
					voc.effect[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<uint64_t>(effectAttribute.value());
				}

				pugi::xml_attribute storageAttribute = childNode.attribute("storage");
				if (storageAttribute) {
					voc.storage[pugi::cast<uint16_t>(transformAttribute.value())] = pugi::cast<uint64_t>(storageAttribute.value());
				}
			}
		}
	}
	return true;
}

Vocation* Vocations::getVocation(uint16_t id)
{
	auto it = vocationsMap.find(id);
	if (it == vocationsMap.end()) {
		std::cout << "[Warning - Vocations::getVocation] Vocation " << id << " not found." << std::endl;
		return nullptr;
	}
	return &it->second;
}

int32_t Vocations::getVocationId(const std::string& name) const
{
	for (const auto& it : vocationsMap) {
		if (strcasecmp(it.second.name.c_str(), name.c_str()) == 0) {
			return it.first;
		}
	}
	return -1;
}

uint16_t Vocations::getPromotedVocation(uint16_t vocationId) const
{
	for (const auto& it : vocationsMap) {
		if (it.second.fromVocation == vocationId && it.first != vocationId) {
			return it.first;
		}
	}
	return VOCATION_NONE;
}

uint32_t Vocation::skillBase[SKILL_LAST + 1] = {50, 50, 50, 50, 30, 100, 20};

uint64_t Vocation::getReqSkillTries(uint8_t skill, uint16_t level)
{
	if (skill > SKILL_LAST) {
		return 0;
	}

	auto it = cacheSkill[skill].find(level);
	if (it != cacheSkill[skill].end()) {
		return it->second;
	}

	uint64_t tries = static_cast<uint64_t>(skillBase[skill] * std::pow(static_cast<double>(skillMultipliers[skill]), level - 11));
	cacheSkill[skill][level] = tries;
	return tries;
}

uint64_t Vocation::getReqMana(uint32_t magLevel)
{
	auto it = cacheMana.find(magLevel);
	if (it != cacheMana.end()) {
		return it->second;
	}

	uint64_t reqMana = static_cast<uint64_t>(1600 * std::pow<double>(manaMultiplier, static_cast<int32_t>(magLevel) - 1));

	cacheMana[magLevel] = reqMana;
	return reqMana;
}