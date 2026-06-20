// termacs — JNI shim. Thin bridge from sciens.termacs.Native to the flat C ABI.
// Statically linked with the C ABI + core + tvision into one libtermacsjni.so.
// Threading: dispatch is single-threaded (§11.6), so callbacks fire on the thread
// that called appRun()/feed — already attached. We GetEnv, attaching only if needed.
#include <jni.h>
#include "termacs/c/termacs.h"
#include <string>

static JavaVM* g_vm = nullptr;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) { g_vm = vm; return JNI_VERSION_1_6; }

namespace {
struct CbRef { jobject obj; };   // global ref to a Native.Cb

JNIEnv* env() {
    JNIEnv* e = nullptr;
    if (g_vm->GetEnv((void**)&e, JNI_VERSION_1_6) != JNI_OK) g_vm->AttachCurrentThread((void**)&e, nullptr);
    return e;
}
void invokeCb(CbRef* r, int i, const char* text) {
    JNIEnv* e = env();
    jclass cls = e->GetObjectClass(r->obj);
    jmethodID m = e->GetMethodID(cls, "invoke", "(ILjava/lang/String;)V");
    jstring s = text ? e->NewStringUTF(text) : nullptr;
    e->CallVoidMethod(r->obj, m, (jint)i, s);
    if (s) e->DeleteLocalRef(s);
    e->DeleteLocalRef(cls);
}
void slotTramp(const TmEvent* ev, void* user)  { invokeCb((CbRef*)user, ev ? ev->i : 0, ev ? ev->text : nullptr); }
void thunkTramp(void* user)                     { invokeCb((CbRef*)user, 0, nullptr); }
CbRef* mkRef(JNIEnv* e, jobject cb)             { auto* r = new CbRef(); r->obj = e->NewGlobalRef(cb); return r; }
std::string S(JNIEnv* e, jstring s) {
    if (!s) return {};
    const char* c = e->GetStringUTFChars(s, nullptr);
    std::string r = c ? c : "";
    e->ReleaseStringUTFChars(s, c);
    return r;
}
} // namespace

#define APP ((tm_app*)app)
#define W   ((tm_widget)w)

extern "C" {

#define JFN(ret, name) JNIEXPORT ret JNICALL Java_sciens_termacs_Native_##name

JFN(jint, abiVersion)(JNIEnv*, jclass)            { return tm_abi_version(); }
JFN(jint, lastStatus)(JNIEnv*, jclass)            { return tm_last_status(); }
JFN(jstring, statusMessage)(JNIEnv* e, jclass)    { return e->NewStringUTF(tm_status_message()); }

JFN(jlong, appNewHeadless)(JNIEnv*, jclass, jint w, jint h) { return (jlong)tm_app_new_headless(w, h); }
JFN(jlong, appNewTerminal)(JNIEnv*, jclass)       { return (jlong)tm_app_new_terminal(); }
JFN(void, appFree)(JNIEnv*, jclass, jlong app)    { tm_app_free(APP); }
JFN(void, appSetTheme)(JNIEnv*, jclass, jlong app, jint t) { tm_app_set_theme(APP, (TmTheme)t); }
JFN(jint, appRun)(JNIEnv*, jclass, jlong app)     { return tm_app_run(APP); }
JFN(void, appQuit)(JNIEnv*, jclass, jlong app, jint c) { tm_app_quit(APP, c); }
JFN(void, appPost)(JNIEnv* e, jclass, jlong app, jobject cb) { tm_app_post(APP, thunkTramp, mkRef(e, cb)); }
JFN(jlong, appStartTimer)(JNIEnv* e, jclass, jlong app, jint ms, jobject cb) { return (jlong)tm_app_start_timer(APP, ms, thunkTramp, mkRef(e, cb)); }
JFN(void, appStopTimer)(JNIEnv*, jclass, jlong app, jlong id) { tm_app_stop_timer(APP, (uint64_t)id); }
JFN(void, appRender)(JNIEnv*, jclass, jlong app)  { tm_app_render(APP); }
JFN(jint, appScreenW)(JNIEnv*, jclass, jlong app) { int w = 0, h = 0; tm_app_screen_size(APP, &w, &h); return w; }
JFN(jint, appScreenH)(JNIEnv*, jclass, jlong app) { int w = 0, h = 0; tm_app_screen_size(APP, &w, &h); return h; }
JFN(jstring, appRowText)(JNIEnv* e, jclass, jlong app, jint y) { char b[1024]; tm_app_row_text(APP, y, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, appFeedKey)(JNIEnv*, jclass, jlong app, jint k, jint ch) { tm_app_feed_key(APP, (TmKey)k, (uint32_t)ch); }

JFN(jlong, createWindow)(JNIEnv* e, jclass, jlong app, jstring t) { auto s = S(e, t); return (jlong)tm_create_window(APP, s.c_str()); }
JFN(void, windowResize)(JNIEnv*, jclass, jlong app, jlong w, jint ww, jint hh) { tm_window_resize(APP, W, ww, hh); }
JFN(jlong, windowSetMenubar)(JNIEnv*, jclass, jlong app, jlong w)     { return (jlong)tm_window_set_menubar(APP, W); }
JFN(jlong, windowSetStatusbar)(JNIEnv*, jclass, jlong app, jlong w)   { return (jlong)tm_window_set_statusbar(APP, W); }
JFN(jlong, windowSetContentVbox)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_window_set_content_vbox(APP, W); }
JFN(jlong, windowSetContentHbox)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_window_set_content_hbox(APP, W); }
JFN(void, windowShow)(JNIEnv*, jclass, jlong app, jlong w)           { tm_window_show(APP, W); }

JFN(void, containerSetPadding)(JNIEnv*, jclass, jlong app, jlong w, jint c) { tm_container_set_padding(APP, W, c); }
JFN(void, containerSetSpacing)(JNIEnv*, jclass, jlong app, jlong w, jint c) { tm_container_set_spacing(APP, W, c); }
JFN(jlong, addLabel)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)  { auto s = S(e, t); return (jlong)tm_add_label(APP, W, s.c_str()); }
JFN(jlong, addButton)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); return (jlong)tm_add_button(APP, W, s.c_str()); }
JFN(jlong, addLineEdit)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_add_lineedit(APP, W); }
JFN(jlong, addListView)(JNIEnv*, jclass, jlong app, jlong w) { return (jlong)tm_add_listview(APP, W); }
JFN(jlong, addHBox)(JNIEnv*, jclass, jlong app, jlong w)     { return (jlong)tm_add_hbox(APP, W); }
JFN(jlong, addVBox)(JNIEnv*, jclass, jlong app, jlong w)     { return (jlong)tm_add_vbox(APP, W); }

JFN(void, widgetSetSizing)(JNIEnv*, jclass, jlong app, jlong w, jint ax, jint mn, jint pf, jint mx, jint st) { tm_widget_set_sizing(APP, W, (TmAxis)ax, mn, pf, mx, st); }
JFN(void, widgetSetFocus)(JNIEnv*, jclass, jlong app, jlong w) { tm_widget_set_focus(APP, W); }
JFN(void, widgetRemove)(JNIEnv*, jclass, jlong app, jlong w)   { tm_widget_remove(APP, W); }

JFN(void, labelSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)    { auto s = S(e, t); tm_label_set_text(APP, W, s.c_str()); }
JFN(void, buttonSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)   { auto s = S(e, t); tm_button_set_text(APP, W, s.c_str()); }
JFN(void, lineEditSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_lineedit_set_text(APP, W, s.c_str()); }
JFN(jstring, lineEditText)(JNIEnv* e, jclass, jlong app, jlong w) { char b[1024]; tm_lineedit_text(APP, W, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, listViewAddItem)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_listview_add_item(APP, W, s.c_str()); }
JFN(void, listViewRemoveItem)(JNIEnv*, jclass, jlong app, jlong w, jint row) { tm_listview_remove_item(APP, W, row); }
JFN(jint, listViewCount)(JNIEnv*, jclass, jlong app, jlong w)    { return tm_listview_count(APP, W); }
JFN(jint, listViewSelected)(JNIEnv*, jclass, jlong app, jlong w) { return tm_listview_selected(APP, W); }
JFN(jstring, listViewItemAt)(JNIEnv* e, jclass, jlong app, jlong w, jint row) { char b[1024]; tm_listview_item_at(APP, W, row, b, sizeof b); return e->NewStringUTF(b); }

JFN(jlong, menubarAddMenu)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); return (jlong)tm_menubar_add_menu(APP, W, s.c_str()); }
JFN(void, menuAddItem)(JNIEnv* e, jclass, jlong app, jlong w, jstring t, jobject cb) { auto s = S(e, t); tm_menu_add_item(APP, W, s.c_str(), slotTramp, mkRef(e, cb)); }
JFN(void, statusbarSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)      { auto s = S(e, t); tm_statusbar_set_text(APP, W, s.c_str()); }
JFN(void, statusbarSetRightText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_statusbar_set_right_text(APP, W, s.c_str()); }

JFN(void, onClicked)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)     { tm_on_clicked(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, onSubmitted)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)   { tm_on_submitted(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, onTextChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_text_changed(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, onActivated)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)   { tm_on_activated(APP, W, slotTramp, mkRef(e, cb)); }

JFN(void, dialogInfo)(JNIEnv* e, jclass, jlong app, jlong w, jstring m)              { auto s = S(e, m); tm_dialog_info(APP, W, s.c_str()); }
JFN(void, dialogConfirm)(JNIEnv* e, jclass, jlong app, jlong w, jstring m, jobject cb) { auto s = S(e, m); tm_dialog_confirm(APP, W, s.c_str(), slotTramp, mkRef(e, cb)); }

// ----- P5 selection & input widgets (§5.10) -----
JFN(void, buttonSetVariant)(JNIEnv*, jclass, jlong app, jlong w, jint v)     { tm_button_set_variant(APP, W, v); }
JFN(void, buttonSetDefault)(JNIEnv*, jclass, jlong app, jlong w, jboolean d) { tm_button_set_default(APP, W, d ? 1 : 0); }

JFN(jlong, addCheckBox)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)    { auto s = S(e, t); return (jlong)tm_add_checkbox(APP, W, s.c_str()); }
JFN(void, checkBoxSetChecked)(JNIEnv*, jclass, jlong app, jlong w, jboolean c) { tm_checkbox_set_checked(APP, W, c ? 1 : 0); }
JFN(jboolean, checkBoxIsChecked)(JNIEnv*, jclass, jlong app, jlong w)        { return tm_checkbox_is_checked(APP, W) ? JNI_TRUE : JNI_FALSE; }
JFN(void, onToggled)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb)      { tm_on_toggled(APP, W, slotTramp, mkRef(e, cb)); }

JFN(jlong, addOptionGroup)(JNIEnv*, jclass, jlong app, jlong w, jint mode)   { return (jlong)tm_add_optiongroup(APP, W, mode); }
JFN(void, optionGroupSetOptions)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_optiongroup_set_options(APP, W, s.c_str()); }
JFN(void, optionGroupSetOrientation)(JNIEnv*, jclass, jlong app, jlong w, jint ax) { tm_optiongroup_set_orientation(APP, W, (TmAxis)ax); }
JFN(void, optionGroupSetSelectedIndex)(JNIEnv*, jclass, jlong app, jlong w, jint i) { tm_optiongroup_set_selected_index(APP, W, i); }
JFN(jint, optionGroupSelectedIndex)(JNIEnv*, jclass, jlong app, jlong w)     { return tm_optiongroup_selected_index(APP, W); }
JFN(void, optionGroupSetSelected)(JNIEnv*, jclass, jlong app, jlong w, jint i, jboolean on) { tm_optiongroup_set_selected(APP, W, i, on ? 1 : 0); }
JFN(jboolean, optionGroupIsSelected)(JNIEnv*, jclass, jlong app, jlong w, jint i) { return tm_optiongroup_is_selected(APP, W, i) ? JNI_TRUE : JNI_FALSE; }
JFN(void, onOptionChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_option_changed(APP, W, slotTramp, mkRef(e, cb)); }

JFN(jlong, addComboBox)(JNIEnv*, jclass, jlong app, jlong w)                 { return (jlong)tm_add_combobox(APP, W); }
JFN(void, comboBoxSetOptions)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_combobox_set_options(APP, W, s.c_str()); }
JFN(void, comboBoxSetSelectedIndex)(JNIEnv*, jclass, jlong app, jlong w, jint i) { tm_combobox_set_selected_index(APP, W, i); }
JFN(jint, comboBoxSelectedIndex)(JNIEnv*, jclass, jlong app, jlong w)        { return tm_combobox_selected_index(APP, W); }
JFN(jstring, comboBoxSelectedText)(JNIEnv* e, jclass, jlong app, jlong w)    { char b[1024]; tm_combobox_selected_text(APP, W, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, comboBoxSetPlaceholder)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_combobox_set_placeholder(APP, W, s.c_str()); }
JFN(void, onComboChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_combo_changed(APP, W, slotTramp, mkRef(e, cb)); }

JFN(jlong, addProgressBar)(JNIEnv*, jclass, jlong app, jlong w)             { return (jlong)tm_add_progressbar(APP, W); }
JFN(void, progressBarSetValue)(JNIEnv*, jclass, jlong app, jlong w, jint v) { tm_progressbar_set_value(APP, W, v); }
JFN(jint, progressBarValue)(JNIEnv*, jclass, jlong app, jlong w)            { return tm_progressbar_value(APP, W); }

JFN(jlong, addTextArea)(JNIEnv*, jclass, jlong app, jlong w)                { return (jlong)tm_add_textarea(APP, W); }
JFN(void, textAreaSetText)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_textarea_set_text(APP, W, s.c_str()); }
JFN(jstring, textAreaText)(JNIEnv* e, jclass, jlong app, jlong w)           { char b[8192]; tm_textarea_text(APP, W, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, textAreaAppendLine)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_textarea_append_line(APP, W, s.c_str()); }
JFN(void, textAreaSetReadOnly)(JNIEnv*, jclass, jlong app, jlong w, jboolean r) { tm_textarea_set_readonly(APP, W, r ? 1 : 0); }
JFN(void, textAreaSetWordWrap)(JNIEnv*, jclass, jlong app, jlong w, jboolean wr) { tm_textarea_set_wordwrap(APP, W, wr ? 1 : 0); }
JFN(void, textAreaSetPlaceholder)(JNIEnv* e, jclass, jlong app, jlong w, jstring t) { auto s = S(e, t); tm_textarea_set_placeholder(APP, W, s.c_str()); }
JFN(void, onTextAreaChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_textarea_changed(APP, W, slotTramp, mkRef(e, cb)); }

JFN(jlong, addFrame)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)      { auto s = S(e, t); return (jlong)tm_add_frame(APP, W, s.c_str()); }
JFN(void, frameSetTitle)(JNIEnv* e, jclass, jlong app, jlong w, jstring t)  { auto s = S(e, t); tm_frame_set_title(APP, W, s.c_str()); }
JFN(jlong, addScrollView)(JNIEnv*, jclass, jlong app, jlong w)              { return (jlong)tm_add_scrollview(APP, W); }
JFN(void, scrollViewScrollTo)(JNIEnv*, jclass, jlong app, jlong w, jint x, jint y) { tm_scrollview_scroll_to(APP, W, x, y); }

// ----- text-editing surface (§5.11) -----
JFN(void, lineEditSelectAll)(JNIEnv*, jclass, jlong app, jlong w)           { tm_lineedit_select_all(APP, W); }
JFN(jstring, lineEditSelectedText)(JNIEnv* e, jclass, jlong app, jlong w)   { char b[8192]; tm_lineedit_selected_text(APP, W, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, lineEditCopy)(JNIEnv*, jclass, jlong app, jlong w)                { tm_lineedit_copy(APP, W); }
JFN(void, lineEditCut)(JNIEnv*, jclass, jlong app, jlong w)                 { tm_lineedit_cut(APP, W); }
JFN(void, lineEditPaste)(JNIEnv*, jclass, jlong app, jlong w)               { tm_lineedit_paste(APP, W); }
JFN(void, onLineEditSelectionChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_lineedit_selection_changed(APP, W, slotTramp, mkRef(e, cb)); }
JFN(void, textAreaSelectAll)(JNIEnv*, jclass, jlong app, jlong w)           { tm_textarea_select_all(APP, W); }
JFN(jstring, textAreaSelectedText)(JNIEnv* e, jclass, jlong app, jlong w)   { char b[8192]; tm_textarea_selected_text(APP, W, b, sizeof b); return e->NewStringUTF(b); }
JFN(void, textAreaCopy)(JNIEnv*, jclass, jlong app, jlong w)                { tm_textarea_copy(APP, W); }
JFN(void, textAreaCut)(JNIEnv*, jclass, jlong app, jlong w)                 { tm_textarea_cut(APP, W); }
JFN(void, textAreaPaste)(JNIEnv*, jclass, jlong app, jlong w)               { tm_textarea_paste(APP, W); }
JFN(void, onTextAreaSelectionChanged)(JNIEnv* e, jclass, jlong app, jlong w, jobject cb) { tm_on_textarea_selection_changed(APP, W, slotTramp, mkRef(e, cb)); }

} // extern "C"
