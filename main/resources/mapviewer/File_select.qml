import QtQuick 2.11
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

RowLayout{
    id: cty_fil_select
    Layout.alignment: Qt.AlignTop
    Layout.maximumHeight: 6
    
    property string tag : tg
    property string tit :  txt
    property string defo : def_folder
    property string is_xml : xml
    property bool geo: grid
    property bool dta: dta
    
    Label{
        id: cty_file_label
        text: tit
        verticalAlignment: Text.AlignVCenter
        Layout.minimumWidth: 245
        Layout.alignment: Qt.AlignLeft
        
    }
    
    TextField{
        id: cty_config_text
        Layout.alignment: Qt.AlignRight
        Layout.minimumWidth: 300
        Layout.preferredWidth: 450
        Layout.maximumWidth: 600
        text: backend.readPath(parent.tag, geo, dta)
        
        
    }
    
    Button{
        text: "..."
        onClicked:{
            cty_file.visible = true
        }
        Layout.maximumWidth: 20
    }
    
    FileDialog{
        id: cty_file
        title: "Please choose a file"
        folder: def_folder
        nameFilters: xml ?["*.xml"] : [ "*.csv"]
        onAccepted: {
            cty_config_text.text = backend.setPath(cty_fil_select.tag, cty_file.fileUrls.toString());
            
        }
        onRejected: {
            
        }
        
    }
}
