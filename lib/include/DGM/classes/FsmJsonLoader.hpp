#include <DGM/classes/FsmLoader.hpp>

namespace dgm
{

	namespace fsm
	{
		/**
		 *  Class for loading FSM definitions from JSON
		 *
		 *  This can be plugged into fsm::Factory to load JSONs that conform to following schema:
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "type": "array",
  "items": [
	{
	  "type": "object",
	  "properties": {
		"name": {
		  "type": "string"
		},
		"transitions": {
		  "type": "array",
		  "items": [
			{
			  "type": "object",
			  "properties": {
				"condition": {
				  "type": "string"
				},
				"target": {
				  "type": "string"
				}
			  },
			  "required": [
				"condition",
				"target"
			  ]
			}
		  ]
		},
		"behaviors": {
		  "type": "array",
		  "items": [
			{
			  "type": "string"
			},
		  ]
		},
		"defaultTransition": {
		  "type": "string"
		}
	  },
	  "required": [
		"name",
		"transitions",
		"behaviors",
		"defaultTransition"
	  ]
	}
  ]
}
		 */
		class JsonLoader final : public LoaderInterface
		{
		public:
			[[nodiscard]]
			virtual loader::StateMap loadFromFile(const std::string&) const override;

			[[nodiscard]]
			loader::StateMap
				loadFromStream(std::istream&) const;
		};
	}

}