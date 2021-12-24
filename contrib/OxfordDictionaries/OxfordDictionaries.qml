import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("Oxford Dictionaries")
    description: qsTr("Oxford Dictionaries uses data from https://developer.oxforddictionaries.com.")
    property url url: "https://od-api.oxforddictionaries.com/api/v2/"
    property string appId
    property string appKey
    property bool strictMatch: false
    property string tl: "en-us"
    property var axiosInstance

    onQuery: {
        const reqDef = axiosInstance.get(`entries/${tl}/${text}?fields=definitions&strictMatch=${strictMatch}`)
        const reqPron = axiosInstance.get(`entries/${tl}/${text}?fields=pronunciations&strictMatch=${strictMatch}`)
        axios.all([reqDef, reqPron]).then(axios.spread((...responses) => {
                let result = {"engine": name, "text": text, "type": "lookup", "definitions": []}
                const lexicalEntries = responses[0].data.results[0].lexicalEntries
                const respPron = responses[1].data.results[0].lexicalEntries
                let pronunciations = {}
				for (const lexicalEntry of respPron) {
                    let pronunciation = lexicalEntry.entries[0].pronunciations[1]
                    pronunciations[lexicalEntry.lexicalCategory.id] =
                        {
                            "text": `/${pronunciation.phoneticSpelling}/`,
                            "audio": pronunciation.audioFile
                        }
                }
				for (const lexicalEntry of lexicalEntries) {
                    let definition = {"partOfSpeech": lexicalEntry.lexicalCategory.id, "list": [], "phonetic": pronunciations[lexicalEntry.lexicalCategory.id]}
                    for (const entry of lexicalEntry.entries) {
                        for (const sense of entry.senses) {
                            definition.list.push({"definition": sense.definitions[0]})
                            if (typeof sense.subsenses !== "undefined") {
                                for (const subSense of sense.subsenses) {
                                    definition.list.push({"definition": subSense.definitions[0]})
                                }
                            }
                        }
                    }
                    result.definitions.push(definition)
                }
                queryResult(result)
            }))
            .catch(function (error) {
                console.log("Oxford Dictionaries:", error)
            })
    }

    Component.onCompleted: {
        axiosInstance = axios.create({
            baseURL: String(url),
            headers: {"app_id": appId, "app_key": appKey}
            })
    }
}
