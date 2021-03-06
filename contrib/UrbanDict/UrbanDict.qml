import QtQuick 2.15
import com.quickdict.components 1.0

Dict {
    name: qsTr("Urban Dictionary")
    description: qsTr("Urban Dictionary uses data from https://www.urbandictionary.com.")
    property url url: "https://api.urbandictionary.com/v0/define?term="

    onQuery: {
        axios.get(url + text)
            .then(function (response) {
                if (!response.data.list.length)
                    return

                let result = {"engine": name, "text": response.data.list[0].word, "type": "lookup"}
                let definitions = {"list": []}
                for (const entry of response.data.list) {
                    definitions.list.push({"definition": entry.definition, "examples": entry.example})
                }
                result.definitions = [definitions]
                queryResult(result)
            })
            .catch(function (error) {
                console.log("UrbanDict:", error)
            })
    }
}
