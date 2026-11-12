/* M07: edit the game's own Binding objects, preserving its typing/gamepad gates. */
#include "../../runtime/native/gameplay.h"
#include "support.h"
#include "../../runtime/native/input.h"
static const char *actions[]={"Interact","Action","Capsule","RotationLock","Use","Secondary","Jump","CycleView","Map","HelmetLamp","HowToPlay","Scan","GravityBoots","Scoreboard","Home","Pause","Back","Boost","Work","Lights","Cargo","Drive","Flip","Cruise","BuildMenu","Remove"};
#define ACTIONS (sizeof(actions)/sizeof(*actions))
static struct {DRoot binding,keys,label;PTR keys_field,label_field,mouse_field;int mouse;} saved[ACTIONS];
static void (*static_get)(PTR,void *),(*set_field)(PTR,PTR,void *);
static PTR (*new_array)(PTR,U64),(*field_type)(PTR);
static int installed;
static int resolve_mod_binding(const char *name,char *text,unsigned capacity){
    if(capacity<2)return 0;
    DWORD n=sx.ini_string("mod-bindings",name,"",text,capacity,sx.config);
    if(n>=capacity-1){
        /* A truncated chord may itself parse as a different valid chord. Return
         * an invalid override so the registry retains its last valid value. */
        text[0]='?';text[1]=0;sx_event("mod-binding-too-long",0);return 1;
    }
    return n!=0;
}
static void restore(void){
    for(unsigned i=0;i<ACTIONS;i++){
        PTR b=dget(saved[i].binding);if(b){PTR keys=dget(saved[i].keys),label=dget(saved[i].label);
            set_field(b,saved[i].keys_field,keys);set_field(b,saved[i].label_field,label);set_field(b,saved[i].mouse_field,&saved[i].mouse);
        }
        ddrop(&saved[i].keys);ddrop(&saved[i].label);ddrop(&saved[i].binding);
    }
    installed=0;
}
static int shape(PTR field,const char *expected){char *name=field?da.type_name(field_type(field)):0;int match=name&&deq(name,expected);if(name)da.free(name);return match;}
static void apply(void){
    PTR input=find_class("MoonCleaners","MoonInput"),key_class=find_class("UnityEngine.InputSystem","Key");int applied=0;
    for(unsigned i=0;i<ACTIONS;i++){
        char text[80];DWORD n=sx.ini_string("bindings",actions[i],"",text,sizeof(text),sx.config);if(!n)continue;
        if(n>=sizeof(text)-1){sx_event("binding-too-long",(int)i);continue;}
        PTR source=get_field(input,actions[i]),b=0;if(!source){sx_event("missing-action",(int)i);continue;}static_get(source,&b);if(!b)continue;
        PTR klass=object_class(b),kf=get_field(klass,"keys"),lf=get_field(klass,"<KeyLabel>k__BackingField"),mf=get_field(klass,"mouse");
        if(!shape(kf,"UnityEngine.InputSystem.Key[]")||!shape(lf,"System.String")||!shape(mf,"System.Int32")){sx_event("binding-shape-rejected",(int)i);continue;}
        int mouse=-1,key=0;
        if(deq(text,"LeftMouse"))mouse=0;else if(deq(text,"RightMouse"))mouse=1;else if(deq(text,"MiddleMouse"))mouse=2;
        else{PTR enum_field=get_field(key_class,text);
            /* value__ is an instance Int32 field, not a Key enum constant. */
            if(!shape(enum_field,"UnityEngine.InputSystem.Key")){sx_event("unknown-key",(int)i);continue;}static_get(enum_field,&key);}
        PTR array=new_array(key_class,mouse<0?1:0);if(!array){sx_event("key-array-failed",(int)i);continue;}
        DRoot array_root=dhold(array);if(!array_root){sx_event("key-root-failed",(int)i);continue;}
        if(mouse<0)*(int *)((unsigned char *)dget(array_root)+32)=key;
        PTR keys=0,label=0;field_value(b,kf,&keys);field_value(b,lf,&label);
        saved[i].binding=dhold(b);saved[i].keys=dhold(keys);saved[i].label=dhold(label);
        saved[i].keys_field=kf;saved[i].label_field=lf;saved[i].mouse_field=mf;field_value(b,mf,&saved[i].mouse);
        if(!saved[i].binding||(keys&&!saved[i].keys)||(label&&!saved[i].label)){
            ddrop(&saved[i].binding);ddrop(&saved[i].keys);ddrop(&saved[i].label);ddrop(&array_root);sx_event("binding-root-failed",(int)i);continue;
        }
        PTR new_label=da.string(text);array=dget(array_root);b=dget(saved[i].binding);
        if(!new_label){
            ddrop(&saved[i].binding);ddrop(&saved[i].keys);ddrop(&saved[i].label);ddrop(&array_root);
            sx_event("key-label-failed",(int)i);continue;
        }
        /* Reference fields receive objects; only the scalar mouse index takes
         * an address. Stack addresses here corrupt the game's input bindings. */
        set_field(b,kf,array);set_field(b,lf,new_label);set_field(b,mf,&mouse);ddrop(&array_root);applied++;
    }
    moon_input_api()->configure(resolve_mod_binding);
    installed=1;sx_event("bindings-applied",applied);
}
static int update(int id,Context *c){
    (void)id;if(!sx_thread())return 0;PTR walker=(PTR)c->rcx;int ok=1;PTR world=sx_world(walker,&ok);
    int input_ok=1,allowed=world&&ok&&sx_input(walker,&input_ok)&&input_ok;
    MoonInputState reload=moon_input_action("custom-keybinds.reload","Ctrl+Home",allowed);
    MoonInputState reset=moon_input_action("custom-keybinds.reset","Ctrl+End",allowed);
    if(!world||!ok){if(installed){restore();moon_input_api()->configure(0);}return 0;}
    if(!installed)apply();
    if(reload.pressed){restore();apply();}
    if(reset.pressed){restore();moon_input_api()->configure(0);installed=1;sx_event("bindings-restored",0);}
    return 0;
}
__declspec(dllexport) int MoonLab_ModStart(PTR domain,PTR game){
    (void)integer;
    if(!sx_start(domain,game,"custom-keybinds","MOONLAB_KEYBINDS_CONFIG"))return 0;
    static_get=(void(*)(PTR,void *))GetProcAddress(game,"il2cpp_field_static_get_value");
    set_field=(void(*)(PTR,PTR,void *))GetProcAddress(game,"il2cpp_field_set_value");
    field_type=(PTR(*)(PTR))GetProcAddress(game,"il2cpp_field_get_type");new_array=(PTR(*)(PTR,U64))GetProcAddress(game,"il2cpp_array_new");
    if(!static_get||!set_field||!field_type||!new_array||!find_class("UnityEngine.InputSystem","Key"))return 0;
    if(!sx_install(game,update))return 0;sx_event("ready",0);return 1;
}
