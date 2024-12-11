#include "util/fm_interaction.hh"

bool FMInteraction::OpenFolderAndSelectFile(const wxString& path) {
#if  defined(_WIN32)
    ITEMIDLIST* pidl = nullptr;

    HRESULT pdn_res = SHParseDisplayName(path, nullptr, &pidl, 0, nullptr);
    if (FAILED(pdn_res) && pidl == nullptr) {
        return false;
    }

    HRESULT ofsi_res = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
    if (FAILED(ofsi_res)) {
        return false;
    }

    CoTaskMemFree(pidl);
    
    return true;
#elif defined(__APPLE__) 
    // Experimental implementation - may change 
    wxString reveal_comm = wxString::Format("open -R \"%s\"", path.utf8_string());
    long comm_exec_res = wxExecute(reveal_comm);

    return comm_exec_res != -1;
#elif defined(__linux__) 
    DBusError        error;
    DBusMessageIter  args, array_iter;
    DBusConnection*  connection = nullptr;
    DBusMessage*     message    = nullptr;
    DBusPendingCall* pending    = nullptr;

    if (!path) {
        return false;
    }

    std::string uri_path = DirectoryUtil::Linux_URLEncode(path.ToStdString());

    dbus_error_init(&error);

    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
        dbus_error_free(&error);
        return false;
    }
    if (!connection) {
        return false;
    }

    message = dbus_message_new_method_call(
        "org.freedesktop.FileManager1",     // Destination (service)
        "/org/freedesktop/FileManager1",    // Path
        "org.freedesktop.FileManager1",     // Interface
        "ShowItems"                         // Method
    );
    if (!message) {
        dbus_connection_unref(connection);
        return false;
    }

    dbus_message_iter_init_append(message, &args);
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "s", &array_iter);

    const char* cstr_uri_path = uri_path.c_str();
    if (!dbus_message_iter_append_basic(&array_iter, DBUS_TYPE_STRING, &cstr_uri_path)) {
        dbus_message_unref(message);
        dbus_connection_unref(connection);
        return false;
    }

    dbus_message_iter_close_container(&args, &array_iter);

    const char* dbus_iter_args = "";
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dbus_iter_args)) {
        dbus_message_unref(message);
        dbus_connection_unref(connection);
        return false;
    }

    if (!dbus_connection_send(connection, message, nullptr)) {
        dbus_message_unref(message);
        dbus_connection_unref(connection);
        return false;
    }

    dbus_message_unref(message);
    dbus_connection_unref(connection);

    return true;
#endif
}