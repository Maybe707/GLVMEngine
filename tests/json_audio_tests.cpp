#include "JsonReader.hpp"
#include "SoundEngineAlsa.hpp"
#include <cassert>
#include <fstream>
#include <thread>

int main() {
    using namespace GLVM::Core;
    auto value = JsonReader(R"({"exponent":1E-3,"values":[1,2,{"ok":true}],"text":"quote: \" \u0410 \ud83d\ude00"})").parse();
    assert(value["exponent"].isFloat() && std::abs(value["exponent"].value.fNumber - 0.001) < 1e-12);
    assert(value["values"][2]["ok"].value.boolean);
    assert(*value["text"].value.string == "quote: \" \xD0\x90 \xF0\x9F\x98\x80");
    JsonValue* self = &value;
    value = *self;
    assert(value["values"][1].value.iNumber == 2);
    for (const char* bad : {"", "{", "[1,]", "{\"a\":}", "[truefalse]", "01", "1e", "1e9999", "\"unclosed", "\"\\ud800\"", "null true"}) {
        bool rejected = false;
        try { JsonReader(bad).parse(); } catch (const std::runtime_error&) { rejected = true; }
        assert(rejected);
    }
    HashMap<int> map;
    map["ab"] = 1;
    map["ba"] = 2;
    assert(map.SearchKey("ba"));
    HashMap<int> copy;
    copy["old"] = 10;
    copy = map;
    HashMap<int>* same = &copy;
    copy = *same;
    assert(copy["ab"] == 1 && copy["ba"] == 2 && !copy.Contain("old"));
    assert(copy["new"] == 0);
    CJsonParser parser;
    parser.ReadFile("gltf/hyper_cube.gltf");
    parser.Parse();
    assert(parser.GetRoot()->isObject());
    parser.ReadFile("gltf/scene.gltf");
    parser.Parse();
    assert((*parser.GetRoot())["meshes"].isArray());
    bool missing = false;
    try { parser.ReadFile("build/does-not-exist.json"); } catch (const std::runtime_error&) { missing = true; }
    assert(missing && parser.GetRoot() == nullptr);
    {
        GLVM::core::Sound::CSoundEngineAlsa audio;
        audio.CloseDevice();
        audio.OpenDevice("null");
        GLVM::core::Sound::CSoundSample missingSample{"build/does-not-exist.wav", 1, 22050, 1.0f};
        audio.PlaybackSoundSample(missingSample);
        std::thread producer([&audio] {
            for (int i = 0; i < 20; ++i)
                audio.CreateSoundSample("laser2.wav", 1, 22050, 0.01f);
        });
        for (int i = 0; i < 20; ++i) audio.SoundStream();
        producer.join();
        audio.CloseDevice();
        audio.CloseDevice();
    }
    std::cout << "PASS JSON syntax, model reload, HashMap ownership and concurrent audio\n";
}
