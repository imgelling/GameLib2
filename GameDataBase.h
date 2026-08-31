#pragma once
#include <vector>
#include <unordered_map>
#include <streambuf>
#include <stack>
#include <utility>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>

namespace game
{
	namespace DataBase
	{
		bool StringToUI64(const std::string& str, uint64_t& result) 
		{
			result = 0;
			if (str.empty()) return false;

			for (char c : str) 
			{
				if (c < '0' || c > '9')
				{
					// Not a number
					return false; 
				}
				const uint64_t digit = (uint64_t)c - '0';

				// Check for overflow before multiplying
				if (result > (UINT64_MAX - digit) / 10) return false;

				result = result * 10 + digit;
			}
			return true;
		}

		struct DataBaseValue
		{
			std::string value;
			std::vector<std::string> comments;
		};

		struct DataBaseProperty
		{
			std::string name;
			std::vector<std::string> comments;
			std::vector<DataBaseValue> values;
		};

		class DataBaseObject
		{
		public:
			std::string name;
			std::vector<std::string> comments;
			std::unordered_map<std::string, uint64_t> objectIndex;
			std::vector<DataBaseObject> objects;
			std::unordered_map<std::string, uint64_t> propertyIndex;
			std::vector<DataBaseProperty> properties;

			DataBaseObject(const std::string& basename = "root")
				: name(basename)
			{}

			inline bool ObjectExists(const std::string& inName) const noexcept
			{
				// See if object exists already
				return (objectIndex.count(inName) > 0);
			}



			// Will return the actual object if it exists,
			// nullptr if a .object.etc does not exist
			// or the object before the last dot
			inline DataBaseObject* operator[](const std::string& id)
			{
				static std::string nextName;
				static std::string newName;
				const uint64_t loc = id.find_first_of('.');
				if (loc != std::string::npos)
				{
					// found a dot
					//std::cout << "[" << name << "]" << std::endl; // for testing

					// Next object name
					nextName = id.substr(0, loc);
					// Possible next object name
					newName = id.substr(loc + 1, id.size());
					if (ObjectExists(nextName))
					{
						return objects[objectIndex[nextName]].operator[](newName);
					}
					else
					{
						return nullptr;
					}
				}
				else
				{
					//std::cout << "[" << name << "]" << std::endl; // for testing
					if (ObjectExists(id))
					{
						return &objects[objectIndex[id]];
					}
					else
						return this;
				}
			}

			bool AddComment(const std::string& location, const std::string& comment)
			{
				//std::string finalComment = (comment);
				std::string finalThing = _GetLastDot(location);
				DataBaseObject* object = operator[](location);

				// object can be bad if last thing before the last dot is not an object
				// meaning it is a value
				if (finalThing.empty())
				{
					finalThing = this->name;
				}
				if (!object)
				{
					std::cout << "[" << location << "] was not an object" << std::endl;
					DataBaseProperty* prop = Property(location);
					if (!prop)
					{
						std::cout << "Property [" << location << "] does not exist" << std::endl;
						return false;
					}
					else
					{
						if (prop->name == finalThing)
						{
							std::cout << "[" << location << "] was a property" << std::endl;
							prop->comments.emplace_back(comment);
							return true;
						}
						std::cout << "[" << location << "] was not a property" << std::endl;
						uint64_t index = 0;
						std::stringstream ss;
						ss << finalThing;
						ss >> index;
						std::cout << "Index of value is " << index << std::endl;
						if (index >= 0 && index < prop->values.size())
						{
							std::cout << "[" << location << "] was a value" << std::endl;
							prop->values[index].comments.emplace_back(comment);
						}
						else
						{
							std::cout << "[" << location << "] was an invalid value" << std::endl;
						}
						return true;
					}
					return false;
				}
				else
				{
					if (object->name == finalThing)
					{
						std::cout << "[" << location << "] was an object" << std::endl;
						object->comments.emplace_back(comment);
					}
					else
					{
						std::cout << "[" << location << "] was not an object" << std::endl;
						std::cout << "Testing if property...\n";
						DataBaseProperty* prop = Property(location);
						if (prop)
						{
							std::cout << "[" << location << "] was a property" << std::endl;
							prop->comments.emplace_back(comment);
							return true;
						}
						else
						{
							std::cout << "[" << location << "] was not a property" << std::endl;
							return false;
						}
					}
				}

				return true;

			}

			bool AddObject(const std::string& inName)
			{
				DataBaseObject newObject;
				newObject.name = _GetLastDot(inName);
				DataBaseObject* object = operator[](inName);;

				if (!object)
				{
					std::cout << "DataBaseObject [" << inName << "] does not exist" << std::endl;
					return false;
				}

				//if (object->ObjectExists(newObject.name))
				if (object->name == newObject.name)
				{
					std::cout << "DataBaseObject [" << inName << "] already exists" << std::endl;
					return false;
				}
				object->objectIndex.insert({ newObject.name, object->objects.size() });
				object->objects.emplace_back(newObject);
				return true;
			}

			bool AddValue(const std::string& propertyName, const std::string& value)
			{
				DataBaseProperty* prop = Property(propertyName);
				if (!prop)
				{
					std::cout << "Property [" << propertyName << "] does not exist" << std::endl;
					return false;
				}
				prop->values.emplace_back(value);
				return true;
			}

			bool AddProperty(const std::string& propertyName, const std::string value = "")
			{
				DataBaseProperty newProp;
				newProp.name = _GetLastDot(propertyName);
				DataBaseObject* object = operator[](propertyName);

				if (!object)
				{
					std::cout << "DataBaseObject [" << propertyName << "] does not exist" << std::endl;
					return false;
				}

				if (!object->PropertyExists(newProp.name))
				{
					auto res = object->propertyIndex.insert({ newProp.name, object->properties.size() });
					object->properties.emplace_back(newProp);
					//std::cout << "----------------------- property index " << object->propertyIndex[newProp.name] << "\n"; // testing
					if (value.size())
					{
						DataBaseProperty& prop = object->properties[object->propertyIndex[newProp.name]];
						//std::cout << "Adding value to " << prop.name << "\n"; // used for testing
						prop.values.emplace_back(value);
					}
				}
				else
				{
					std::cout << "Property |" << propertyName << "| already exists" << std::endl;
					return false;
				}
				return true;
			}

			inline bool PropertyExists(const std::string& propertyName) const noexcept
			{
				return (propertyIndex.count(propertyName));
			}

			std::string ToString()
			{
				std::streambuf* originalCoutBuffer = std::cout.rdbuf();
				std::stringstream ss;
				std::cout.rdbuf(ss.rdbuf());
				PrintObject(*this);
				std::cout.rdbuf(originalCoutBuffer);
				return ss.str();
			}

			// # text is comment
			// [text] = object
			// |text| = property
			// text inside property = value;
			// objects and properties will be printed in order created
			// all properties will be written before objects
			inline void PrintObject(const DataBaseObject& obj, const int32_t indents = 0, const std::string& indentString = "\t")
			{
				const auto indent = [](const int32_t count, const std::string& indentString)
					{
						for (int32_t i = 0; i < count; ++i)
							std::cout << indentString;
					};
				for (const auto& comment : obj.comments)
				{
					indent(indents, indentString);
					std::cout << "# " << comment << std::endl;
				}
				indent(indents, indentString);
				std::cout << "[" + obj.name + "]" << std::endl;
				indent(indents, indentString);
				std::cout << "{" << std::endl;
				for (auto& prop : obj.properties)
				{
					for (const auto& comment : prop.comments)
					{
						indent(indents + 1, indentString);
						std::cout << "# " << comment << std::endl;
					}
					indent(indents + 1, indentString);
					std::cout << "|" << prop.name << "|" << std::endl;
					//std::cout << prop.name << " from " << obj.name << std::endl; // used for testing
					indent(indents + 1, indentString);
					std::cout << "{" << std::endl;
					for (const auto& values : prop.values)
					{
						for (const auto& comment : values.comments)
						{
							indent(indents + 2, indentString);
							std::cout << "# " << comment << std::endl;
						}
						indent(indents + 2, indentString);
						std::cout << values.value << std::endl;
					}
					indent(indents + 1, indentString);
					std::cout << "}" << std::endl;
				}
				for (const auto& sub : obj.objects)
				{
					PrintObject(sub, indents + 1, indentString);
				}
				indent(indents - 1 + 1, indentString);
				std::cout << "}" << std::endl;
			}

			void PrintObjectsAndProperties()
			{
				PrintObject(*this);
			}

			// Will return a pointer to the last thing that was a proper Property
			DataBaseProperty* Property(const std::string& property)
			{
				uint64_t loc = property.find_first_of('.');
				static std::string nextName;
				static std::string newName;
				if (loc != std::string::npos)
				{
					// found a dot
					nextName = property.substr(0, loc);
					//std::cout << "[" << nextName << "]" << std::endl; // used for testing
					newName = property.substr(loc + 1, property.size());
					if (newName.find_first_of('.') == std::string::npos)
					{
						// this is the last object
						// check if this is a property
						if (PropertyExists(nextName))
						{
							return &properties[propertyIndex[nextName]];
						}
						if (!ObjectExists(nextName))
						{
							return nullptr;
						}
						if (!objects[objectIndex[nextName]].PropertyExists(newName))
						{
							return nullptr;
						}
						else
						{
							DataBaseObject& temp = objects[objectIndex[nextName]];
							uint64_t index = temp.propertyIndex[newName];
							//std::cout << "Property index = " << index << "\n";
							//std::cout << "Property at that index is " << temp.properties[index].name << "\n";
							return &temp.properties[index];
							//return  &objects[objectIndex[nextName]].properties[objects[objectIndex[nextName]].propertyIndex[newName]];
						}
					}
					return objects[objectIndex[nextName]].Property(newName);
				}
				else
				{
					// this is the last one and should be property
					// check for existing
					if (PropertyExists(property))
					{
						return &properties[propertyIndex[property]];
					}
					else
					{
						return nullptr;
					}
				}
			}

			bool LoadFromString(const std::string& str)
			{
				std::stringstream ss(str);
				std::string line;
				std::string_view lineSV;
				std::vector<std::string> commentsRead;
				std::stack <std::pair<DataBaseObject*, int64_t>> objectStack;
				std::pair<DataBaseProperty*, int64_t> currentPropertyDepth;
				int64_t depth = 0;
				std::string property;
				std::string object;
				DataBaseObject* temp = nullptr;

				objectStack.push({ this,0 });
				do
				{
					std::getline(ss, line);
					lineSV = line;
					//lineSV = lineSV.substr(line.find_first_not_of(" \t\n\r\f\v"), line.find_last_not_of(" \t\n\r\f\v") + 1);
					lineSV.remove_prefix(lineSV.find_first_not_of(" \t\n\r\f\v"));
					auto dkfj = lineSV.find_last_not_of(" \t\n\r\f\v")+1;
					lineSV.remove_suffix(lineSV.size() - dkfj);
					//std::cout << "Parsing line \n" << lineSV << "\n";
					//line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
					//line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
					//line.resize(line.find_last_not_of(" \t\n\r\f\v")+1);
					//if (!lineSV.size())
					//{
					//	std::cout << "shit\n";
					//}
					if (lineSV[0] == '{')
					{
						depth++;
						continue;
					}
					if (lineSV[0] == '}')
					{
						depth--;
						if (objectStack.top().second == depth)
						{
							if (objectStack.size() > 1) objectStack.pop();
							//std::cout << "Found '}' , Stack top is " << objectStack.top().first->name << "\n";
						}
						if (currentPropertyDepth.second == depth)
						{
							currentPropertyDepth.first = nullptr;
							currentPropertyDepth.second = 0;
						}
						continue;
					}
					// Read the comments, we will assign later
					if (lineSV[0] == '#')
					{
						if (!(lineSV.size() - 1))
						{
							std::cout << "Empty comment found\n";
							//line.clear();
							lineSV.remove_prefix(line.size());// = "";

						}
						else
						{
							//lineSV = lineSV.substr(2, line.size());
							lineSV.remove_prefix(2);
							//lineSV.remove_suffix()
						}
						//std::cout << "Comment found # " << line << "\n";
						commentsRead.emplace_back(std::string(lineSV));
						continue;
					}

					// Should be an Object
					if (lineSV[0] == '[')
					{
						// Object found
						//std::cout << "Object " << line << " found\n";
						if (lineSV.back() != ']')
						{
							std::cout << "Malformed data. Object has no closing ']'.\n";
							return false;
						}
						object = lineSV.substr(1, lineSV.size() - 2);
						if (!object.size())
						{
							std::cout << "Malformed data. Object is unnamed.\n";
							return false;
						}

						if (depth == 0) //object == "root")
						{
							for (auto& com : commentsRead)
							{
								comments.emplace_back(com);
							}
							commentsRead.clear();
							continue;
						}
						// Add the object
						objectStack.top().first->AddObject(object);
						// Add to the stack to "enter" the object
						temp = objectStack.top().first;
						objectStack.push({ &temp->objects[temp->objectIndex[object]], depth });
						//std::cout << "Stack top is " << objectStack.top().first->name << "\n";
						for (auto& com : commentsRead)
						{
							objectStack.top().first->comments.emplace_back(com);
						}
						commentsRead.clear();

						continue;
					}

					// Should be a property
					if (lineSV[0] == '|')
					{
						// Property found
						//std::cout << "Property " << line << " found\n";
						if (lineSV.back() != '|')
						{
							std::cout << "Malformed data. Property has no closing '|'.\n";
							return false;
						}
						property = lineSV.substr(1, lineSV.size() - 2);
						if (!property.size())
						{
							std::cout << "Malformed data. Property is unnamed.\n";
							return false;
						}
						temp = objectStack.top().first;
						temp->AddProperty(property);
						currentPropertyDepth.first = &temp->properties[temp->propertyIndex[property]];// ()Property(property);
						currentPropertyDepth.second = depth;
						for (auto& com : commentsRead)
						{
							currentPropertyDepth.first->comments.emplace_back(com);
						}
						commentsRead.clear();
						continue;
					}

					// Should be a value
					{
						if (currentPropertyDepth.first)
						{
							//std::cout << "Value \"" << line << "\" found\n";
							if (!lineSV.size())
							{
								std::cout << "Malformed data. Value has no data. Just a blank line\n";
								return false;
							}
							// it is a value
							currentPropertyDepth.first->values.emplace_back(std::string(lineSV));
							for (auto& com : commentsRead)
							{
								DataBaseProperty* p = currentPropertyDepth.first;
								DataBaseValue* v = &p->values[p->values.size() - 1];
								v->comments.emplace_back(com);
							}
							commentsRead.clear();
						}
						else
						{
							std::cout << "Malformed data. Value on line not in a property.\n";
							return false;
						}
					}
				} while (ss.rdbuf()->in_avail());
				if (depth)
				{
					std::cout << "Malformed data. Closing '}' do not equal Opening '{'\n";
					return false;
				}
				return true;
			}

			void Clear()
			{
				objectIndex.clear();
				objects.clear();
				comments.clear();
				propertyIndex.clear();
				properties.clear();
			}




		private:
			std::string _GetLastDot(const std::string& id) const noexcept
			{
				const uint64_t loc = id.find_last_of('.');
				if (loc == std::string::npos)
				{
					return id;
				}
				return id.substr(loc + 1, id.size());
			}
		};
	}
}