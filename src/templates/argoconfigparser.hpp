//
// Created by bla on 04/06/20.
//


//     ArgoWorkflowConfigParsed data = nlohmann::json::parse(jsonString);




#ifndef ADES_CODE_ENGINE_API_FRAMEWORK_ARGOCONFIGPARSER_HPP
#define ADES_CODE_ENGINE_API_FRAMEWORK_ARGOCONFIGPARSER_HPP

#include <nlohmann/json.hpp>

#include <optional>
#include <stdexcept>
#include <regex>



namespace proc_ades_argo {
    namespace model {
        using nlohmann::json;

        inline json get_untyped(const json & j, const char * property) {
            if (j.find(property) != j.end()) {
                return j.at(property).get<json>();
            }
            return json();
        }

        inline json get_untyped(const json & j, std::string property) {
            return get_untyped(j, property.data());
        }

        template <typename T>
        inline std::shared_ptr<T> get_optional(const json & j, const char * property) {
            if (j.find(property) != j.end()) {
                return j.at(property).get<std::shared_ptr<T>>();
            }
            return std::shared_ptr<T>();
        }

        template <typename T>
        inline std::shared_ptr<T> get_optional(const json & j, std::string property) {
            return get_optional<T>(j, property.data());
        }

        class Credential {
        public:
            Credential() = default;
            virtual ~Credential() = default;

        private:
            std::shared_ptr<std::string> user;
            std::shared_ptr<std::string> password;

        public:
            std::shared_ptr<std::string> get_user() const { return user; }
            void set_user(std::shared_ptr<std::string> value) { this->user = value; }

            std::shared_ptr<std::string> get_password() const { return password; }
            void set_password(std::shared_ptr<std::string> value) { this->password = value; }
        };

        class In {
        public:
            In() = default;
            virtual ~In() = default;

        private:
            std::shared_ptr<std::string> docker;
            std::shared_ptr<Credential> credential;

        public:
            std::shared_ptr<std::string> get_docker() const { return docker; }
            void set_docker(std::shared_ptr<std::string> value) { this->docker = value; }

            std::shared_ptr<Credential> get_credential() const { return credential; }
            void set_credential(std::shared_ptr<Credential> value) { this->credential = value; }
        };

        class Out {
        public:
            Out() = default;
            virtual ~Out() = default;

        private:
            std::shared_ptr<std::string> docker;
            std::shared_ptr<std::string> webdav_endpoint;
            std::shared_ptr<Credential> credential;

        public:
            std::shared_ptr<std::string> get_docker() const { return docker; }
            void set_docker(std::shared_ptr<std::string> value) { this->docker = value; }

            std::shared_ptr<std::string> get_webdav_endpoint() const { return webdav_endpoint; }
            void set_webdav_endpoint(std::shared_ptr<std::string> value) { this->webdav_endpoint = value; }

            std::shared_ptr<Credential> get_credential() const { return credential; }
            void set_credential(std::shared_ptr<Credential> value) { this->credential = value; }
        };

        class Stage {
        public:
            Stage() = default;
            virtual ~Stage() = default;

        private:
            std::shared_ptr<Credential> credential;
            std::shared_ptr<In> in;
            std::shared_ptr<Out> out;

        public:
            std::shared_ptr<Credential> get_credential() const { return credential; }
            void set_credential(std::shared_ptr<Credential> value) { this->credential = value; }

            std::shared_ptr<In> get_in() const { return in; }
            void set_in(std::shared_ptr<In> value) { this->in = value; }

            std::shared_ptr<Out> get_out() const { return out; }
            void set_out(std::shared_ptr<Out> value) { this->out = value; }
        };

        class ArgoWorkflowConfigParsed {
        public:
            ArgoWorkflowConfigParsed() = default;
            virtual ~ArgoWorkflowConfigParsed() = default;

        private:
            std::shared_ptr<std::string> argopath;
            std::shared_ptr<std::string> k8_url;
            std::shared_ptr<std::string> argo_url;
            std::shared_ptr<Stage> stage;

        public:
            std::shared_ptr<std::string> get_argopath() const { return argopath; }
            void set_argopath(std::shared_ptr<std::string> value) { this->argopath = value; }

            std::shared_ptr<std::string> get_k8_url() const { return k8_url; }
            void set_k8_url(std::shared_ptr<std::string> value) { this->k8_url = value; }

            std::shared_ptr<std::string> get_argo_url() const { return argo_url; }
            void set_argo_url(std::shared_ptr<std::string> value) { this->argo_url = value; }

            std::shared_ptr<Stage> get_stage() const { return stage; }
            void set_stage(std::shared_ptr<Stage> value) { this->stage = value; }
        };
    }
}

namespace nlohmann {
    void from_json(const json & j, proc_ades_argo::model::Credential & x);
    void to_json(json & j, const proc_ades_argo::model::Credential & x);

    void from_json(const json & j, proc_ades_argo::model::In & x);
    void to_json(json & j, const proc_ades_argo::model::In & x);

    void from_json(const json & j, proc_ades_argo::model::Out & x);
    void to_json(json & j, const proc_ades_argo::model::Out & x);

    void from_json(const json & j, proc_ades_argo::model::Stage & x);
    void to_json(json & j, const proc_ades_argo::model::Stage & x);

    void from_json(const json & j, proc_ades_argo::model::ArgoWorkflowConfigParsed & x);
    void to_json(json & j, const proc_ades_argo::model::ArgoWorkflowConfigParsed & x);

    inline void from_json(const json & j, proc_ades_argo::model::Credential& x) {
        x.set_user(proc_ades_argo::model::get_optional<std::string>(j, "user"));
        x.set_password(proc_ades_argo::model::get_optional<std::string>(j, "password"));
    }

    inline void to_json(json & j, const proc_ades_argo::model::Credential & x) {
        j = json::object();
        j["user"] = x.get_user();
        j["password"] = x.get_password();
    }

    inline void from_json(const json & j, proc_ades_argo::model::In& x) {
        x.set_docker(proc_ades_argo::model::get_optional<std::string>(j, "docker"));
        x.set_credential(proc_ades_argo::model::get_optional<proc_ades_argo::model::Credential>(j, "credential"));
    }

    inline void to_json(json & j, const proc_ades_argo::model::In & x) {
        j = json::object();
        j["docker"] = x.get_docker();
        j["credential"] = x.get_credential();
    }

    inline void from_json(const json & j, proc_ades_argo::model::Out& x) {
        x.set_docker(proc_ades_argo::model::get_optional<std::string>(j, "docker"));
        x.set_webdav_endpoint(proc_ades_argo::model::get_optional<std::string>(j, "webdav_endpoint"));
        x.set_credential(proc_ades_argo::model::get_optional<proc_ades_argo::model::Credential>(j, "credential"));
    }

    inline void to_json(json & j, const proc_ades_argo::model::Out & x) {
        j = json::object();
        j["docker"] = x.get_docker();
        j["webdav_endpoint"] = x.get_webdav_endpoint();
        j["credential"] = x.get_credential();
    }

    inline void from_json(const json & j, proc_ades_argo::model::Stage& x) {
        x.set_credential(proc_ades_argo::model::get_optional<proc_ades_argo::model::Credential>(j, "credential"));
        x.set_in(proc_ades_argo::model::get_optional<proc_ades_argo::model::In>(j, "in"));
        x.set_out(proc_ades_argo::model::get_optional<proc_ades_argo::model::Out>(j, "out"));
    }

    inline void to_json(json & j, const proc_ades_argo::model::Stage & x) {
        j = json::object();
        j["credential"] = x.get_credential();
        j["in"] = x.get_in();
        j["out"] = x.get_out();
    }

    inline void from_json(const json & j, proc_ades_argo::model::ArgoWorkflowConfigParsed& x) {
        x.set_argopath(proc_ades_argo::model::get_optional<std::string>(j, "argopath"));
        x.set_k8_url(proc_ades_argo::model::get_optional<std::string>(j, "k8Url"));
        x.set_argo_url(proc_ades_argo::model::get_optional<std::string>(j, "argoUrl"));
        x.set_stage(proc_ades_argo::model::get_optional<proc_ades_argo::model::Stage>(j, "stage"));
    }

    inline void to_json(json & j, const proc_ades_argo::model::ArgoWorkflowConfigParsed & x) {
        j = json::object();
        j["argopath"] = x.get_argopath();
        j["k8Url"] = x.get_k8_url();
        j["argoUrl"] = x.get_argo_url();
        j["stage"] = x.get_stage();
    }
}

#endif