#include "abstracttool.h"

#include <QtCore/QCoreApplication>

namespace TextureTool {

/*!
    \class AbstractTool
    This is the base class for texture tools.
*/

/*!
    \fn AbstractTool::AbstractTool()
    Constructs an AbstractTool instance.
*/

/*!
    Destroys the AbstractTool object.
*/
AbstractTool::~AbstractTool() = default;

/*!
    \fn QByteArray AbstractTool::id() const
    Reimplement this function to return the id of the tool.
    This id is used a command name when invoking main program: ./texturetool \<id\> [options]
*/

/*!
    \fn QString AbstractTool::decription() const
    Reimplement this function to return the decription of the tool.
    Description is used when help of the main program is printed.
*/

/*!
    \fn int AbstractTool::run(const QStringList &arguments)
    Reimplement this function to implement the logic of the tool.

    When tool is invoked via ./texturetool \<id\> [options], \a arguments contains options passed
    to the tool. Tool should return 0 in case of success or non-zero in case of an error.
    Also, it can throw an exception that is caught by the main program.
*/

} // namespace TextureTool
