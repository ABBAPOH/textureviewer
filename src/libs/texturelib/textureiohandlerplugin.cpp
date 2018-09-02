#include "textureiohandlerplugin.h"

/*!
    \class TextureIOHandlerPlugin

    Base class for texture handler plugins.

    Plugins serve as factories for TextureIOHandler objects.

    Each plugin can support multiple mime types with different capabilities.
*/

/*!
    \enum TextureIOHandlerPlugin::Capability
    This enum describes capabities of the handler.

    \var TextureIOHandlerPlugin::Capability TextureIOHandlerPlugin::CanRead
    Whether handler can read

    \var TextureIOHandlerPlugin::Capability TextureIOHandlerPlugin::CanWrite
    Whether handler can write

    \var TextureIOHandlerPlugin::Capability TextureIOHandlerPlugin::ReadWrite
    Whether handler can read and write
*/

/*!
    \fn TextureIOHandlerPlugin::TextureIOHandlerPlugin()

    Constructs a new TextureIOHandlerPlugin object.
*/

/*!
    \fn TextureIOHandlerPlugin::Capabilities TextureIOHandlerPlugin::capabilities(QStringView mimeType)

    Reimplement this function to return the capabilities of the plugin for the given \a mimeType name.
*/

/*!
    \fn std::unique_ptr<TextureIOHandler> TextureIOHandlerPlugin::create(QStringView mimeType)

    Reimplement this function to create a newly constructed handler for the given \a mimeType name.
*/
